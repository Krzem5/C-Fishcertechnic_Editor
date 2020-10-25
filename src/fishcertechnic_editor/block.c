#include <block.h>
#include <editor.h>
#include <stdio.h>



BlockArray blk_l={
	0,
	NULL
};



void init_block_list(void){
	assert(blk_l.ln==0&&blk_l.e==NULL);
	WIN32_FIND_DATAA fdt;
	HANDLE fh=FindFirstFileA("data\\*",&fdt);
	assert(fh!=INVALID_HANDLE_VALUE);
	while (true){
		if ((fdt.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0){
			uint64_t sz=((uint64_t)fdt.nFileSizeHigh<<32)|fdt.nFileSizeLow;
			uint16_t fpl=(uint16_t)strlen(fdt.cFileName);
			char* fp=malloc(fpl+6);
			*fp='d';
			*(fp+1)='a';
			*(fp+2)='t';
			*(fp+3)='a';
			*(fp+4)='\\';
			for (size_t i=0;i<fpl;i++){
				*(fp+i+5)=*(fdt.cFileName+i);
			}
			*(fp+fpl+5)=0;
			FILE* f=NULL;
			assert(fopen_s(&f,fp,"rb")==0);
			blk_l.ln++;
			blk_l.e=realloc(blk_l.e,blk_l.ln*sizeof(struct __BLOCK));
			(blk_l.e+blk_l.ln-1)->fl=(uint8_t)fgetc(f);
			(blk_l.e+blk_l.ln-1)->id=(((blk_l.e+blk_l.ln-1)->fl&0x3)<<16)|((uint8_t)fgetc(f)<<8)|(uint8_t)fgetc(f);
			(blk_l.e+blk_l.ln-1)->nml=(uint8_t)fgetc(f);
			(blk_l.e+blk_l.ln-1)->nm=malloc((blk_l.e+blk_l.ln-1)->nml*sizeof(char)+1);
			assert(fread((blk_l.e+blk_l.ln-1)->nm,1,(blk_l.e+blk_l.ln-1)->nml*sizeof(char),f)==(blk_l.e+blk_l.ln-1)->nml*sizeof(char));
			*((blk_l.e+blk_l.ln-1)->nm+(blk_l.e+blk_l.ln-1)->nml)=0;
			(blk_l.e+blk_l.ln-1)->r=(uint8_t)fgetc(f);
			(blk_l.e+blk_l.ln-1)->g=(uint8_t)fgetc(f);
			(blk_l.e+blk_l.ln-1)->b=(uint8_t)fgetc(f);
			(blk_l.e+blk_l.ln-1)->_fpl=fpl+6;
			(blk_l.e+blk_l.ln-1)->_fp=fp;
			printf("ID: %llu, NM: `%s', RGB: #%02hhx%02hhx%02hhx\n",(blk_l.e+blk_l.ln-1)->id,(blk_l.e+blk_l.ln-1)->nm,(blk_l.e+blk_l.ln-1)->r,(blk_l.e+blk_l.ln-1)->g,(blk_l.e+blk_l.ln-1)->b);
			assert(0);
			fclose(f);
		}
		if (FindNextFileA(fh,&fdt)==0){
			break;
		}
	}
	assert(GetLastError()==ERROR_NO_MORE_FILES);
	FindClose(fh);
}
