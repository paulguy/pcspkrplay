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

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <linux/input.h>

#include "output.h"

#include "freqs.h"

char exstr[7][20] = {
	"PROGRAM_ENDED",
	"ILLEGAL_INSTRUCTION",
	"BAD_ARGUMENT",
	"BAD_REGISTER",
	"DEVICE_ERROR",
	"VM_ERROR",
	"BREAKPOINT"
};

char exstrlong[7][52] = {
	"The program reached it's end with no error.",
	"An illegal instruction was encountered.",
	"A bad value was used in the argument field.",
	"A nonexistent register was attempted to be used.",
	"An error with a device occurred.",
	"An error in the VM occurred.",
	"A breakpoint was reached."
};

char cmdstr[22][8] = {
	"NOTE",
	"MOV",
	"ADD",
	"SUB",
	"CMP",
	"JNE",
	"JE",
	"JL",
	"JG",
	"JMP",
	"BRA",
	"RET",
	"CFL",
	"HALT",
	"LABEL",
	"JNEL",
	"JEL",
	"JLL",
	"JGL",
	"JMPL",
	"BRAL",
	"NOP"
};

int playnote(int fd, int note, int octave) {
	struct input_event ev;
	ev.type = EV_SND;
	ev.code = SND_TONE;

	if(octave >= 0 && octave <= 6) {
		if(note == NOTECF) {
			note = NOTEB;
			if(octave > 0)
				octave--;
		} else if(note == NOTEBS) {
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

vmexception playsong(int fd, song *s, vmstate *vm, void (*status)(int cur, int max), int step) {
	int i;
	struct timespec req, rem;
	unsigned long int notelen;
	unsigned int t;
	int running;

	i = 1;
	running = 1;
	while(running == 1) {
		if(status != NULL)
			status(i, s->count);

		vm->last = s->current;

		switch(s->current->cmd) {
			case NOTE:
					if((s->current->data & 0xFF) == 0xFF) {
						t = IMMEDTOINT(s->current->data);
						if(playnote(fd, IMMEDTOINT(s->current->data), vm->regs[REGOCTAVE]) < 0)
							return(DEVICE_ERROR);
					} else if(s->current->data < 26) {
						if(playnote(fd, vm->regs[s->current->data], vm->regs[REGOCTAVE]) < 0)
							return(DEVICE_ERROR);
					} else {
						return(BAD_ARGUMENT);
					}

				notelen = 60000000000ul / vm->regs[REGBPM] / vm->regs[REGDIVISOR];
				if(vm->regs[REGDURATION] == DDOT) {
					notelen = notelen * 3 / 2;
				} else if(vm->regs[REGDURATION] == DTRIPLET) {
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
					if((s->current->data & 0xFF) == 0xFF) {
						if(s->current->reg == REGDURATION) {
							if(vm->regs[REGDURATION] == IMMEDTOINT(s->current->data)) {
								vm->regs[REGDURATION] = 0;
							} else {
								vm->regs[s->current->reg] = IMMEDTOINT(s->current->data);
							}
						} else {
							vm->regs[s->current->reg] = IMMEDTOINT(s->current->data);
						}
					} else if(s->current->data < 26) {
						if(s->current->reg == REGDURATION) {
							if(vm->regs[REGDURATION] == vm->regs[s->current->data]) {
								vm->regs[REGDURATION] = 0;
							} else {
								vm->regs[s->current->reg] = vm->regs[s->current->data];
							}
						} else {
							vm->regs[s->current->reg] = vm->regs[s->current->data];
						}
					} else {
						return(BAD_ARGUMENT);
					}
				} else {
					return(BAD_REGISTER);
				}
				if(vm->regs[REGOCTAVE] > 6)
					vm->regs[REGOCTAVE] = 6;
				if(vm->regs[REGDIVISOR] < 1)
					vm->regs[REGDIVISOR] = 1;
				if(vm->regs[REGBPM] < 1)
					vm->regs[REGBPM] = 1;
				if(vm->regs[REGDURATION] > 2)
					vm->regs[REGDURATION] = 0;
				break;
			case ADD:
				if(s->current->reg < 26) {
					if((s->current->data & 0xFF) == 0xFF) {
						vm->regs[s->current->reg] += IMMEDTOINT(s->current->data);
					} else if(s->current->data < 26) {
						vm->regs[s->current->reg] += vm->regs[s->current->data];
					} else {
						return(BAD_ARGUMENT);
					}
				} else {
					return(BAD_REGISTER);
				}
				if(vm->regs[REGOCTAVE] > 6)
					vm->regs[REGOCTAVE] = 6;
				if(vm->regs[REGDURATION] > 2)
					vm->regs[REGDURATION] = 0;
				break;
			case SUB:
				if(s->current->reg < 26) {
					if((s->current->data & 0xFF) == 0xFF) {
						vm->regs[s->current->reg] -= IMMEDTOINT(s->current->data);
					} else if(s->current->data < 26) {
						vm->regs[s->current->reg] -= vm->regs[s->current->data];
					} else {
						return(BAD_ARGUMENT);
					}
				} else {
					return(BAD_REGISTER);
				}
				if(vm->regs[REGDIVISOR] < 1)
					vm->regs[REGDIVISOR] = 1;
				break;
			case CMP:
				if(s->current->reg < 26) {
					if((s->current->data & 0xFF) == 0xFF) {
						t = IMMEDTOINT(s->current->data);
						if(t > vm->regs[s->current->reg])
							vm->flags |= FLAG_GREATER;
						if(t == vm->regs[s->current->reg])
							vm->flags |= FLAG_EQUAL;
					} else if(s->current->data < 26) {
						if(vm->regs[s->current->data] > vm->regs[s->current->reg])
							vm->flags |= FLAG_GREATER;
						if(vm->regs[s->current->data] == vm->regs[s->current->reg])
							vm->flags |= FLAG_EQUAL;
					} else {
						return(BAD_ARGUMENT);
					}
				} else {
					return(BAD_REGISTER);
				}
				break;
			case JNE:
				if((s->current->data & 0xFF) == 0xFF) {
					if(!(vm->flags & FLAG_EQUAL)) {
						t = IMMEDTOINT(s->current->data);
						seeksong(s, t);
					}
				} else if(s->current->data < 26) {
					if(!(vm->flags & FLAG_EQUAL))
						seeksong(s, vm->regs[s->current->data]);
				} else {
					return(BAD_ARGUMENT);
				}
				break;
			case JE:
				if((s->current->data & 0xFF) == 0xFF) {
					if(vm->flags & FLAG_EQUAL) {
						t = IMMEDTOINT(s->current->data);
						seeksong(s, t);
					}
				} else if(s->current->data < 26) {
					if(vm->flags & FLAG_EQUAL)
						seeksong(s, vm->regs[s->current->data]);
				} else {
					return(BAD_ARGUMENT);
				}
				break;
			case JL:
				if((s->current->data & 0xFF) == 0xFF) {
					if(!(vm->flags & FLAG_GREATER)) {
						t = IMMEDTOINT(s->current->data);
						seeksong(s, t);
					}
				} else if(s->current->data < 26) {
					if(!(vm->flags & FLAG_GREATER))
						seeksong(s, vm->regs[s->current->data]);
				} else {
					return(BAD_ARGUMENT);
				}
				break;
			case JG:
				if((s->current->data & 0xFF) == 0xFF) {
					if(vm->flags & FLAG_GREATER) {
						t = IMMEDTOINT(s->current->data);
						seeksong(s, t);
					}
				} else if(s->current->data < 26) {
					if(vm->flags & FLAG_GREATER)
						seeksong(s, vm->regs[s->current->data]);
				} else {
					return(BAD_ARGUMENT);
				}
				break;
			case JMP:
				if((s->current->data & 0xFF) == 0xFF) {
					t = IMMEDTOINT(s->current->data);
					seeksong(s, t);
				} else if(s->current->data < 26) {
					seeksong(s, vm->regs[s->current->data]);
				} else {
					return(BAD_ARGUMENT);
				}
				break;
			case BRA:
				if(s->current->next == NULL)
					return(ILLEGAL_INSTRUCTION);
				vm->stack[vm->sp] = s->current->next;
				vm->sp++;
				if((s->current->data & 0xFF) == 0xFF) {
					t = IMMEDTOINT(s->current->data);
					seeksong(s, t);
				} else if(s->current->data < 26) {
					seeksong(s, vm->regs[s->current->data]);
				} else {
					return(BAD_ARGUMENT);
				}
				break;
			case JNEL:
				if((s->current->data & 0xFF) == 0xFF) {
					if(!(vm->flags & FLAG_EQUAL)) {
						t = IMMEDTOINT(s->current->data);
						if(t > s->nlabels)
							return(BAD_ARGUMENT);
						seeksong(s, s->labels[t]);
					}
				} else if(s->current->data < 26) {
					if(!(vm->flags & FLAG_EQUAL)) {
						if(vm->regs[s->current->data] > s->nlabels)
							return(BAD_ARGUMENT);
						seeksong(s, s->labels[vm->regs[s->current->data]]);
					}
				} else {
					return(BAD_ARGUMENT);
				}
				break;
			case JEL:
				if((s->current->data & 0xFF) == 0xFF) {
					if(vm->flags & FLAG_EQUAL) {
						t = IMMEDTOINT(s->current->data);
						if(t > s->nlabels)
							return(BAD_ARGUMENT);
						seeksong(s, s->labels[t]);
					}
				} else if(s->current->data < 26) {
					if(vm->flags & FLAG_EQUAL) {
						if(vm->regs[s->current->data] > s->nlabels)
							return(BAD_ARGUMENT);
						seeksong(s, s->labels[vm->regs[s->current->data]]);
					}
				} else {
					return(BAD_ARGUMENT);
				}
				break;
			case JLL:
				if((s->current->data & 0xFF) == 0xFF) {
					if(!(vm->flags & FLAG_GREATER)) {
						t = IMMEDTOINT(s->current->data);
						if(t > s->nlabels)
							return(BAD_ARGUMENT);
						seeksong(s, s->labels[t]);
					}
				} else if(s->current->data < 26) {
					if(!(vm->flags & FLAG_GREATER)) {
						if(vm->regs[s->current->data] > s->nlabels)
							return(BAD_ARGUMENT);
						seeksong(s, s->labels[vm->regs[s->current->data]]);
					}
				} else {
					return(BAD_ARGUMENT);
				}
				break;
			case JGL:
				if((s->current->data & 0xFF) == 0xFF) {
					if(vm->flags & FLAG_GREATER) {
						t = IMMEDTOINT(s->current->data);
						if(t > s->nlabels)
							return(BAD_ARGUMENT);
						seeksong(s, s->labels[t]);
					}
				} else if(s->current->data < 26) {
					if(vm->flags & FLAG_GREATER) {
						if(vm->regs[s->current->data] > s->nlabels)
							return(BAD_ARGUMENT);
						seeksong(s, s->labels[vm->regs[s->current->data]]);
					}
				} else {
					return(BAD_ARGUMENT);
				}
				break;
			case JMPL:
				if((s->current->data & 0xFF) == 0xFF) {
					t = IMMEDTOINT(s->current->data);
					if(t > s->nlabels)
						return(BAD_ARGUMENT);
					seeksong(s, s->labels[t]);
				} else if(s->current->data < 26) {
 					if(vm->regs[s->current->data] > s->nlabels)
						return(BAD_ARGUMENT);
					seeksong(s, s->labels[vm->regs[s->current->data]]);
				} else {
					return(BAD_ARGUMENT);
				}
				break;
			case BRAL:
				if(s->current->next == NULL)
					return(ILLEGAL_INSTRUCTION);
				vm->stack[vm->sp] = s->current->next;
				vm->sp++;
				if((s->current->data & 0xFF) == 0xFF) {
					t = IMMEDTOINT(s->current->data);
					if(t > s->nlabels)
						return(BAD_ARGUMENT);
					seeksong(s, s->labels[t]);
				} else if(s->current->data < 26) {
 					if(vm->regs[s->current->data] > s->nlabels)
						return(BAD_ARGUMENT);
					seeksong(s, s->labels[vm->regs[s->current->data]]);
				} else {
					return(BAD_ARGUMENT);
				}
				break;
			case RET:
				if(vm->sp == 0)
					return(ILLEGAL_INSTRUCTION);
				vm->sp--;
				s->current = vm->stack[vm->sp];
				break;
			case CFL:
				vm->flags = 0;
				break;
			case HALT:
				running = 0;
				break;
			case LABEL:
				break;
			default:
				return(ILLEGAL_INSTRUCTION);
				break;
		}

		i++;
		if(s->current == vm->last)
			if(nextcommand(s) == 1)
				running = 0;

		if(running == 1 && step == 1) {
			return(BREAKPOINT);
		}
	}

	if(playnote(fd, 0, 0) < 0)
		return(DEVICE_ERROR);
	return(PROGRAM_ENDED);
}

void printexception(vmexception e, song *s, vmstate *vm, FILE *out) {
	int i;

	fprintf(out, "VM EXCEPTION: %s: %s\n", exstr[e], exstrlong[e]);
	fprintf(out, "INSTRUCTION:\n");
	if(s->current != NULL) {
		fprintf(out, "  CMD: %s  REG: %c", cmdstr[vm->last->cmd], REGTOLET(vm->last->reg));
		if((vm->last->data & 0xFF) == 0xFF)
			fprintf(out, "  DATA: %x\n", IMMEDTOINT(vm->last->data));
		else
			fprintf(out, "  REG: %c\n", REGTOLET(vm->last->data));
	} else {
		fprintf(out, "At end of song or song is corrupted.\n");
	}
	if(e != VM_ERROR) {
		fprintf(out, "REGISTERS:\n");
		for(i = 0; i < 26; i++)
			fprintf(out, "  %c: %X", REGTOLET(i), vm->regs[i]);
		fprintf(out, "\n");
		fprintf(out, "FLAGS: %X", vm->flags);
		if(vm->flags & FLAG_EQUAL)
			fprintf(out, " EQUAL");
		if(vm->flags & FLAG_GREATER)
			fprintf(out, " GREATER");
		fprintf(out, "  SP: %i\n", vm->sp);
	} else {
		fprintf(out, "VM state may be inconsistent.\n");
	}
}

vmstate *initvm() {
	int i;
	vmstate *vm;

	vm = malloc(sizeof(vmstate));
	if(vm == NULL)
		return(NULL);

	for(i = 0; i < 26; i++) {
		vm->regs[i] = 0;
	}
	vm->flags = 0;

	vm->regs[REGOCTAVE] = DEFAULT_OCTAVE;
	vm->regs[REGDIVISOR] = DEFAULT_DIVISOR;
	vm->regs[REGBPM] = DEFAULT_BPM;
	vm->regs[REGDURATION] = DNORMAL;

	for(i = 0; i < 16; i++) {
		vm->stack[i] = 0;
	}
	vm->sp = 0;

	return(vm);
}
