#include <block.h>
#include <editor.h>
#include <block_pixel.h>
#include <block_vertex.h>
#include <stdio.h>



struct CBufferLayout{
	RawMatrix tm;
	RawMatrix rm;
	RawVector c;
};



BlockArray blk_l={
	0,
	NULL
};
ID3D11VertexShader* b_vs=NULL;
ID3D11PixelShader* b_ps=NULL;
ID3D11Buffer* b_cb=NULL;



void init_block_list(void){
	D3D11_INPUT_ELEMENT_DESC il[]={
		{
			"SV_POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA
		}
	};
	b_vs=load_vertex_shader(g_block_vs,sizeof(g_block_vs),il,sizeof(il)/sizeof(D3D11_INPUT_ELEMENT_DESC));
	b_ps=load_pixel_shader(g_block_ps,sizeof(g_block_ps));
	b_cb=create_constant_buffer(sizeof(struct CBufferLayout));
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
			(blk_l.e+blk_l.ln-1)->id=(((blk_l.e+blk_l.ln-1)->fl&0x1f)<<16)|(uint8_t)fgetc(f)|((uint8_t)fgetc(f)<<8);
			(blk_l.e+blk_l.ln-1)->nml=(uint8_t)fgetc(f);
			(blk_l.e+blk_l.ln-1)->nm=malloc((blk_l.e+blk_l.ln-1)->nml*sizeof(char)+1);
			assert(fread((blk_l.e+blk_l.ln-1)->nm,sizeof(char),(blk_l.e+blk_l.ln-1)->nml,f)==(blk_l.e+blk_l.ln-1)->nml);
			*((blk_l.e+blk_l.ln-1)->nm+(blk_l.e+blk_l.ln-1)->nml)=0;
			(blk_l.e+blk_l.ln-1)->r=(uint8_t)fgetc(f);
			(blk_l.e+blk_l.ln-1)->g=(uint8_t)fgetc(f);
			(blk_l.e+blk_l.ln-1)->b=(uint8_t)fgetc(f);
			(blk_l.e+blk_l.ln-1)->ib=NULL;
			(blk_l.e+blk_l.ln-1)->vb=NULL;
			(blk_l.e+blk_l.ln-1)->ch=((((blk_l.e+blk_l.ln-1)->fl>>4)&0x7)>0?malloc((((blk_l.e+blk_l.ln-1)->fl>>4)&0x7)*sizeof(BlockId)):NULL);
			for (uint8_t i=0;i<((blk_l.e+blk_l.ln-1)->fl>>5);i++){
				*((blk_l.e+blk_l.ln-1)->ch+i)=((uint8_t)fgetc(f)<<16)|(uint8_t)fgetc(f)|((uint8_t)fgetc(f)<<8);
			}
			(blk_l.e+blk_l.ln-1)->ill=(uint8_t)fgetc(f)|((uint8_t)fgetc(f)<<8)|((uint8_t)fgetc(f)<<16)|((uint8_t)fgetc(f)<<24);
			(blk_l.e+blk_l.ln-1)->_fsz=sz;
			(blk_l.e+blk_l.ln-1)->_fp=fp;
			printf("FL: %hhx, ID: %llu, NM: `%s', RGB: #%02hhx%02hhx%02hhx, ILL: %lu, CH: (%hhu) [",(blk_l.e+blk_l.ln-1)->fl,(blk_l.e+blk_l.ln-1)->id,(blk_l.e+blk_l.ln-1)->nm,(blk_l.e+blk_l.ln-1)->r,(blk_l.e+blk_l.ln-1)->g,(blk_l.e+blk_l.ln-1)->b,(blk_l.e+blk_l.ln-1)->ill,(blk_l.e+blk_l.ln-1)->fl>>5);
			for (uint8_t i=0;i<((blk_l.e+blk_l.ln-1)->fl>>5);i++){
				if (i>0){
					printf(", ");
				}
				printf("%llu",*((blk_l.e+blk_l.ln-1)->ch+i));
			}
			printf("]\n");
			fclose(f);
		}
		if (FindNextFileA(fh,&fdt)==0){
			break;
		}
	}
	for (size_t i=0;i<blk_l.ln;i++){
		for (uint8_t j=0;j<((blk_l.e+i)->fl>>5);j++){
			for (size_t k=0;k<blk_l.ln;k++){
				if ((blk_l.e+k)->id==*((blk_l.e+i)->ch+j)){
					*((blk_l.e+i)->ch+j)=k;
					break;
				}
			}
		}
	}
	assert(GetLastError()==ERROR_NO_MORE_FILES);
	FindClose(fh);
}



void draw_block(Block b,Matrix m,Matrix rm,Vector cl){
	ID3D11DeviceContext_VSSetShader(renderer_d3_dc,b_vs,NULL,0);
	ID3D11DeviceContext_PSSetShader(renderer_d3_dc,b_ps,NULL,0);
	struct CBufferLayout cb_dt={
		as_raw_matrix(m),
		as_raw_matrix(rm),
		(cl==NULL?raw_vector(b->r/255.0f,b->g/255.0f,b->b/255.0f,1.0f):as_raw_vector(cl))
	};
	if (b->ib==NULL||b->vb==NULL){
		assert(b->ib==NULL);
		assert(b->vb==NULL);
		printf("Loading Model `%s'...\n",b->nm);
		FILE* f=NULL;
		assert(fopen_s(&f,b->_fp,"rb")==0);
		fpos_t np=b->nml+(b->fl>>5)*3+11;
		fsetpos(f,&np);
		if (/*TMP*/0/*TMP*/==0){
			uint16_t* il=malloc(b->ill*sizeof(uint16_t));
			assert(fread(il,sizeof(uint16_t),b->ill,f)==b->ill);
			D3D11_BUFFER_DESC bd={
				(uint32_t)(b->ill*sizeof(uint16_t)),
				D3D11_USAGE_IMMUTABLE,
				D3D11_BIND_INDEX_BUFFER,
				0,
				0,
				0
			};
			D3D11_SUBRESOURCE_DATA dt={
				il,
				0,
				0
			};
			HRESULT hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&b->ib);
			free(il);
		}
		else{
			assert(0);
		}
		size_t ln=(b->_fsz-b->ill*sizeof(uint16_t)-np)/sizeof(float);
		float* vl=malloc(ln*sizeof(float));
		assert(fread(vl,sizeof(float),ln,f)==ln);
		D3D11_BUFFER_DESC bd={
			(uint32_t)(ln*sizeof(float)),
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_VERTEX_BUFFER,
			0,
			0,
			0
		};
		D3D11_SUBRESOURCE_DATA dt={
			vl,
			0,
			0
		};
		HRESULT hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&b->vb);
		free(vl);
		fclose(f);
	}
	update_constant_buffer(b_cb,(void*)&cb_dt);
	ID3D11DeviceContext_VSSetConstantBuffers(renderer_d3_dc,1,1,&b_cb);
	ID3D11DeviceContext_PSSetConstantBuffers(renderer_d3_dc,1,1,&b_cb);
	unsigned int off=0;
	unsigned int st=7*sizeof(float);
	ID3D11DeviceContext_IASetVertexBuffers(renderer_d3_dc,0,1,&(b->vb),&st,&off);
	ID3D11DeviceContext_IASetIndexBuffer(renderer_d3_dc,b->ib,(/*TMP*/0/*TMP*/==0?DXGI_FORMAT_R16_UINT:DXGI_FORMAT_R32_UINT),0);
	ID3D11DeviceContext_IASetPrimitiveTopology(renderer_d3_dc,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3D11DeviceContext_DrawIndexed(renderer_d3_dc,(uint32_t)b->ill,0,0);
	for (uint8_t i=0;i<(b->fl>>5);i++){
		draw_block(blk_l.e+(*(b->ch+i)),m,rm,NULL);
	}
}
