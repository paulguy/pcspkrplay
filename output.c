/*
 * Copyright (c) 2011 Paul LaMendola
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <unistd.h>
#include <time.h>
#include <linux/input.h>

#include "output.h"

#include "freqs.h"

int playnote(int fd, int note, int octave) {
	struct input_event ev;
	ev.type = EV_SND;
	ev.code = SND_TONE;

	if(octave >= 0 && octave <= 6) {
		if(note == NOTECF) {
			note = NOTEB;
			if(octave > 0)
				octave--;
		} else if(note = NOTEBS) {
			note = NOTEC;
			if(octave < 6)
				octave++;
		}
		ev.value = (int)(notefreqs[octave][note]);
		if(write(fd, &ev, sizeof(struct input_event)) < 0)
			return(-1);
	} else {
		return(-1);
	}
	return(0);
}

vmexception *playsong(int fd, song *s, void (*status)(int cur, int max)) {
	int i;
	struct timespec req, rem;
	unsigned long int notelen;
	struct vmstate vm;

	unsigned int t;

	for(i = 0; i < 26; i++) {
		vm.regs = 0;
	}
	vm.flags = 0;

	vm.regs[REGOCTAVE] = 3;
	vm.regs[REGDIVISOR] = 32;
	vm.regs[REGBPM] = 50;
	vm.regs[REGDURATION] = DNORMAL;

	i = 1;
	do {
		if(status != NULL)
			status(i, s->count);

		switch(s->current->instruction) {
			case NOTE:
				if(playnote(fd, s->current->data, vm.regs[REGOCTAVE]) < 0)
					THROW(DEVICE_ERROR);

				notelen = 60000000000ul / s->bpm / s->current->divisor;
				if(s->current->duration == DOT) {
					notelen = notelen * 3 / 2;
				} else if(s->current->duration == TRIPLET) {
					notelen = notelen / 3;
				}
				req.tv_sec = notelen / 1000000000ul;
				req.tv_nsec = notelen % 1000000000ul;

				while(nanosleep(&req, &rem) == -1) {
					req.tv_sec = rem.tv_sec;
					req.tv_nsec = rem.tv_nsec;
				}
				break;
			case MOV:
				if(s->current->reg < 26) {
					if(s->current->data & 0xFF == 0xFF) {
						t = IMMEDTOINT(s->current->data);
						vm.regs[s->current->reg] = t;
					} else if(s->current->data < 26) {
						vm.regs[s->current->reg] = vm.regs[s->current->data];
					} else {
						THROW(BAD_ARGUMENT);
					}
				} else {
					THROW(BAD_REGISTER);
				}
				break;
			case ADD:
				if(s->current->reg < 26) {
					if(s->current->data & 0xFF == 0xFF) {
						t = IMMEDTOINT(s->current->data);
						vm.regs[s->current->reg] += t;
					} else if(s->current->data < 26) {
						vm.regs[s->current->reg] += vm.regs[s->current->data];
					} else {
						THROW(BAD_ARGUMENT);
					}
				} else {
					THROW(BAD_REGISTER);
				}
				break;
			case SUB:
				if(s->current->reg < 26) {
					if(s->current->data & 0xFF == 0xFF) {
						t = IMMEDTOINT(s->current->data);
						vm.regs[s->current->reg] -= t;
					} else if(s->current->data < 26) {
						vm.regs[s->current->reg] -= vm.regs[s->current->data];
					} else {
						THROW(BAD_ARGUMENT);
					}
				} else {
					THROW(BAD_REGISTER);
				}
				break;
			case CMP:
				if(s->current->reg < 26) {
					if(s->current->data & 0xFF == 0xFF) {
						t = IMMEDTOINT(s->current->data);
						if(t > vm.regs[s->current->reg])
							vm.flags |= FLAG_GREATER;
						if(t == vm.regs[s->current->reg])
							vm.flags |= FLAG_EQUALS;
					} else if(s->current->data < 26) {
						if(vm.regs[s->current->data] > vm.regs[s->current->reg])
							vm.flags |= FLAG_GREATER;
						if(vm.regs[s->current->data] == vm.regs[s->current->reg])
							vm.flags |= FLAG_EQUALS;
					} else {
						THROW(BAD_ARGUMENT);
					}
				} else {
					THROW(BAD_REGISTER);
				}
				break;
			case JNE:
				if(s->current->data & 0xFF == 0xFF) {
					t = IMMEDTOINT(s->current->data);
					if(!(vm.flags & FLAG_EQUALS))
						seeksong(s, t)
				} else if(s->current->data < 26) {
					if(!(vm.flags & FLAG_EQUALS))
						seeksong(s, vm.regs[s->current->data])
				} else {
					THROW(BAD_ARGUMENT);
				}
				break;
			case JE:
				if(s->current->data & 0xFF == 0xFF) {
					t = IMMEDTOINT(s->current->data);
					if(vm.flags & FLAG_EQUALS)
						seeksong(s, t)
				} else if(s->current->data < 26) {
					if(vm.flags & FLAG_EQUALS)
						seeksong(s, vm.regs[s->current->data])
				} else {
					THROW(BAD_ARGUMENT);
				}
				break;
			case JL:
				if(s->current->data & 0xFF == 0xFF) {
					t = IMMEDTOINT(s->current->data);
					if(!(vm.flags & FLAG_GREATER))
						seeksong(s, t)
				} else if(s->current->data < 26) {
					if(!(vm.flags & FLAG_GREATER))
						seeksong(s, vm.regs[s->current->data])
				} else {
					THROW(BAD_ARGUMENT);
				}
				break;
			case JG:
				if(s->current->data & 0xFF == 0xFF) {
					t = IMMEDTOINT(s->current->data);
					if(vm.flags & FLAG_GREATER)
						seeksong(s, t)
				} else if(s->current->data < 26) {
					if(vm.flags & FLAG_GREATER)
						seeksong(s, vm.regs[s->current->data])
				} else {
					THROW(BAD_ARGUMENT);
				}
				break;
			case JMP:
				if(s->current->data & 0xFF == 0xFF) {
					t = IMMEDTOINT(s->current->data);
					seeksong(s, t)
				} else if(s->current->data < 26) {
					seeksong(s, vm.regs[s->current->data])
				} else {
					THROW(BAD_ARGUMENT);
				}
				break;
			case CFL:
				vm.flags = 0;
				break;
			case HALT:
				THROW(PROGRAM_ENDED);
				break;
			default:
				THROW(ILLEGAL_INSTRUCTION);
				break;
		}

		} else {
			switch(s->current->note) {
				case BPM:
					s->bpm = s->current->octave;
					break;
				default:
					break;
			}
		}

		i++;
	} while(nextcommand(s) == 0);

	if(playnote(fd, 0, 0) < 0)
		return(-1);
	THROW(PROGRAM_ENDED);
}

vmexception *mkexception(exception ex, vmstate *vm, song *s) {
	vmexception *e;

	e = malloc(sizeof(vmexception));
	if(e == NULL)
		return(NULL);

	e.e = ex;
	e.vm = vm;
	e.s = s;

	return(e);
}

void printexception(vmexception *e, FILE *out) {
	int i;

	fprintf(out, "VM EXCEPTION: %s: %s\n", exstr[e.e], extrlong[e.e]);
	fprintf(out, "REGISTERS:\n");
	for(i = 0; i < 26; i++)
		fprintf(out, "  %c: %X", i + 'a', e.vm.regs[i]);
	fprintf(out, "FLAGS: %X", e.vm.flags);
	if(e.vm.flags & FLAG_EQUAL)
		fprintf(out, " EQUAL");
	if(e.vm.flags & FLAG_GREATER)
		fprintf(out, " GREATER");
	fprintf(out, "\n");
}
