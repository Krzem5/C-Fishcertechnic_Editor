#include <renderer.h>
#include <editor.h>
#include <ui.h>
#include <color_2d_pixel.h>
#include <color_2d_vertex.h>



ID3D11VertexShader* vs=NULL;
ID3D11PixelShader* ps=NULL;
ObjectBuffer tb=NULL;
ID3D11Buffer* cb=NULL;



struct CBufferLayout{
	RawMatrix pm;
};



void recalc_topbar(void){
	if (tb==NULL){
		tb=create_object_buffer();
	}
	// X Y R G B A  X Y R G B A...
	tb->vll=24;
	tb->vl=realloc(tb->vl,24*sizeof(float));
	#define x renderer_ww/2.0f
	#define y renderer_wh/2.0f
	#define WIDTH 40
	#define HEIGHT 30
	*(tb->vl)=x-WIDTH;
	*(tb->vl+1)=y;
	*(tb->vl+2)=1;
	*(tb->vl+3)=0;
	*(tb->vl+4)=0;
	*(tb->vl+5)=1;
	*(tb->vl+6)=x-WIDTH;
	*(tb->vl+7)=y-HEIGHT;
	*(tb->vl+8)=0;
	*(tb->vl+9)=1;
	*(tb->vl+10)=0;
	*(tb->vl+11)=1;
	*(tb->vl+12)=x;
	*(tb->vl+13)=y-HEIGHT;
	*(tb->vl+14)=0;
	*(tb->vl+15)=0;
	*(tb->vl+16)=1;
	*(tb->vl+17)=1;
	*(tb->vl+18)=x;
	*(tb->vl+19)=y;
	*(tb->vl+20)=1;
	*(tb->vl+21)=0;
	*(tb->vl+22)=1;
	*(tb->vl+23)=1;
	tb->ill=6;
	tb->il=realloc(tb->il,6*sizeof(uint16_t));
	*(tb->il)=0;
	*(tb->il+1)=1;
	*(tb->il+2)=2;
	*(tb->il+3)=0;
	*(tb->il+4)=3;
	*(tb->il+5)=2;
	tb->st=6*sizeof(float);
	update_object_buffer(tb);
}



void init_ui(void){
	D3D11_INPUT_ELEMENT_DESC il[]={
		{
			"SV_POSITION",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA
		},
		{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			8,
			D3D11_INPUT_PER_VERTEX_DATA
		}
	};
	vs=load_vertex_shader(g_color_2d_vs,sizeof(g_color_2d_vs),il,sizeof(il)/sizeof(D3D11_INPUT_ELEMENT_DESC));
	ps=load_pixel_shader(g_color_2d_ps,sizeof(g_color_2d_ps));
	cb=create_constant_buffer(sizeof(struct CBufferLayout));
	recalc_topbar();
}



void update_ui(double dt){
	ID3D11DeviceContext_OMSetDepthStencilState(renderer_d3_dc,renderer_d3_ddss,1);
	ID3D11DeviceContext_VSSetShader(renderer_d3_dc,vs,NULL,0);
	ID3D11DeviceContext_PSSetShader(renderer_d3_dc,ps,NULL,0);
	struct CBufferLayout cb_dt={
		raw_matrix(2.0f/renderer_ww,0,0,0,0,2.0f/renderer_wh,0,0,0,0,1000/(1000-0.1f),1,0,0,-100/(1000-0.1f),0)
	};
	update_constant_buffer(cb,(void*)&cb_dt);
	ID3D11DeviceContext_VSSetConstantBuffers(renderer_d3_dc,0,1,&cb);
	ID3D11DeviceContext_PSSetConstantBuffers(renderer_d3_dc,0,1,&cb);
	draw_object_buffer(tb);
	ID3D11DeviceContext_OMSetDepthStencilState(renderer_d3_dc,renderer_d3_dss,1);
}
