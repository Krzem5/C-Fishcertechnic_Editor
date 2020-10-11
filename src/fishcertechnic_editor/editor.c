#include <editor.h>
#include <renderer.h>
#include <ui.h>
#include <stdio.h>



void run_editor(void){
	init_renderer();
}



void init_editor(void){
	init_ui();
}



void update_editor(double dt){
	static float t=0;
	t+=(float)(dt*1e-6);
	update_ui(dt);
	printf("%f (%f)\n",dt*1e-6,t);
}
