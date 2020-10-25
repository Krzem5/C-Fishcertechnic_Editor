#ifndef BLOCK_H
#define BLOCK_H
#include <renderer.h>
#include <stdint.h>
#include <d3d11_1.h>



typedef struct __BLOCK_ARRAY BlockArray;
typedef struct __BLOCK* Block;
typedef uint64_t BlockId;



struct __BLOCK_ARRAY{
	size_t ln;
	struct __BLOCK* e;
};



struct __BLOCK{
	BlockId id;
	uint8_t nml;
	char* nm;
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t fl;
	uint32_t ill;
	ID3D11Buffer* ib;
	ID3D11Buffer* vb;
	BlockId* ch;
	uint64_t _fsz;
	char* _fp;
};



extern BlockArray blk_l;



void init_block_list(void);



void draw_block(Block b,Matrix tm,Matrix rm,Vector cl);



#endif
