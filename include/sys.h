#ifndef	_SYS_H
#define	_SYS_H

#define SYS_WRITE_NUMBER        0 		// syscal numbers
//#define SYS_MALLOC_NUMBER     1
//#define SYS_CLONE_NUMBER      2
#define SYS_EXIT_NUMBER         1

#ifndef __ASSEMBLER__

void sys_write(char * buf);
//int sys_fork();

#endif
#endif  /*_SYS_H */