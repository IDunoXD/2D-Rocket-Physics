#include <SFML/Graphics.hpp>
#include <iostream>
#include <windows.h>
#include <cmath>
#define PI (float)3.14159265358979323846
using namespace sf;
using namespace std;
class Rocket{
private:
	float default_scale=1;
public:
	float x=0,y=0,r=0,dx=0,dy=0,vY=0,prewdy=0,dr=0,scale=1,time_rotating_left=0,time_rotating_right=0,time_of_thrust=0,time_falling_down=0,
		fire_w_coef=1,fire_h_coef=1,rocket_w_coef=1,rocket_h_coef=1,
		velocity_of_rotation=20/*degree/s*/,velocity_of_thrust=50/*km/h*/,velocity_of_falling=9.8/*m/s*/,velocity_of_destruction=180/*km/h*/,
		acceleration_of_rotation=20/*degree/s*/,acceleration_of_thrust=100/*km/h*/,acceleration_of_falling=19.6/*m/s*/,acceleration_treveling_left=10/*km/h*/,acceleration_treveling_right=10/*km/h*/,
		max_time_of_rotation=5/*seconds*/,max_thrust_speed=1000/*km/h*/,max_falling_speed=1000/*km/h*/,
		default_rocket_width=5/*m*/,default_rocket_height=20/*m*/,default_fire_width=5/*m*/,default_fire_height=5/*m*/;
	bool onGround=false,falling=true,destroyed=false,stats_template_is_drawn=false;
	Texture rocket_texture,fire_texture;
	Sprite rocket,fire,fire_L,fire_R; 
	FloatRect rocket_proportions,fire_proportions;	
	Rocket(float default_scale):default_scale(default_scale){
		velocity_of_thrust*=default_scale/3.6;
		acceleration_of_thrust*=default_scale/3.6;
		acceleration_treveling_left*=default_scale/3.6;
		acceleration_treveling_right*=default_scale/3.6;
		velocity_of_destruction*=default_scale/3.6;
		max_thrust_speed*=default_scale/3.6;
		max_falling_speed*=default_scale/3.6;
		velocity_of_falling*=default_scale;
		acceleration_of_falling*=default_scale;
		rocket_texture.loadFromFile("sprites/R.png");
		fire_texture.loadFromFile("sprites/fire.png");
		rocket.setTexture(rocket_texture);
		fire.setTexture(fire_texture);
		rocket_proportions=rocket.getLocalBounds();
		fire_proportions=fire.getLocalBounds();
		rocket_w_coef=default_rocket_width/(rocket_proportions.width/default_scale);
		rocket_h_coef=default_rocket_height/(rocket_proportions.height/default_scale);
		rocket.setScale(rocket_w_coef,rocket_h_coef);
		fire_w_coef=default_fire_width/(fire_proportions.width/default_scale);
		fire_h_coef=default_fire_height/(fire_proportions.height/default_scale);
		fire.setScale(fire_w_coef,fire_h_coef);
		rocket.setOrigin(rocket_proportions.width/2,rocket_proportions.height/2);
		fire.setOrigin(fire_proportions.width/2,0);
		fire_L=fire_R=fire;
	}
	void update(float& fixed_time,float& scale){
		dy=y;
		dx=x;
		dr=r;
		if(Keyboard::isKeyPressed(Keyboard::Left) &&! (Keyboard::isKeyPressed(Keyboard::Right))){
			time_rotating_left+=fixed_time;
			if(time_rotating_left>max_time_of_rotation)
				time_rotating_left=max_time_of_rotation;
			y-=(acceleration_treveling_left*time_rotating_left)*fixed_time*sin(r*PI/180);
			x-=(acceleration_treveling_left*time_rotating_left)*fixed_time*cos(r*PI/180);
			r-=(velocity_of_rotation+acceleration_of_rotation*time_rotating_left)*fixed_time;
		}else if(time_rotating_left>0){
			y-=(acceleration_treveling_left*time_rotating_left)*fixed_time*sin(r*PI/180);
			x-=(acceleration_treveling_left*time_rotating_left)*fixed_time*cos(r*PI/180);
			r-=(acceleration_of_rotation*time_rotating_left)*fixed_time;
			time_rotating_left-=4*fixed_time;
			if(time_rotating_left<0)
				time_rotating_left=0;
		}
		if(Keyboard::isKeyPressed(Keyboard::Right) &&! (Keyboard::isKeyPressed(Keyboard::Left))){
			time_rotating_right+=fixed_time;
			if(time_rotating_right>max_time_of_rotation)
				time_rotating_right=max_time_of_rotation;
			y+=(acceleration_treveling_right*time_rotating_right)*fixed_time*sin(r*PI/180);
			x+=(acceleration_treveling_right*time_rotating_right)*fixed_time*cos(r*PI/180);
			r+=(velocity_of_rotation+acceleration_of_rotation*time_rotating_right)*fixed_time;
		}else if(time_rotating_right>0){
			y+=(acceleration_treveling_right*time_rotating_right)*fixed_time*sin(r*PI/180);
			x+=(acceleration_treveling_right*time_rotating_right)*fixed_time*cos(r*PI/180);
			r+=(acceleration_of_rotation*time_rotating_right)*fixed_time;
			time_rotating_right-=4*fixed_time;
			if(time_rotating_right<0)
				time_rotating_right=0;
		}
		if(Keyboard::isKeyPressed(Keyboard::Up)){
			time_of_thrust+=fixed_time;
			y-=(velocity_of_thrust+acceleration_of_thrust*time_of_thrust)*fixed_time*cos(r*PI/180);
			x+=(velocity_of_thrust+acceleration_of_thrust*time_of_thrust)*fixed_time*sin(r*PI/180);
		}else if(time_of_thrust>0){
			y-=(acceleration_of_thrust*time_of_thrust)*fixed_time*cos(r*PI/180);
			x+=(acceleration_of_thrust*time_of_thrust)*fixed_time*sin(r*PI/180);
			time_of_thrust-=2*fixed_time;
			if(time_of_thrust<0)
				time_of_thrust=0;
		}
		if(!onGround){
			y+=(velocity_of_falling+acceleration_of_falling*time_falling_down)*fixed_time;
			if(r>0)
				r+=10*time_falling_down*fixed_time;
			else if(r<0)
				r-=10*time_falling_down*fixed_time;
		}
		if(y>1000){
			y=1000;
			onGround=true;
			time_falling_down-=time_of_thrust;
		}else if(y<1000){
			onGround=false;
		}
		dy=y-dy;
		dx=x-dx;
		dr=r-dr;
		if(r>180)
			r-=360;
		else if(r<-180)
			r+=360;
		if(dy/fixed_time>=prewdy && !onGround){
			falling=true;
			time_falling_down+=fixed_time;
		}else{
			falling=false;
			time_falling_down-=fixed_time;
			if(time_falling_down<0)
				time_falling_down=0;
		}
		rocket.setPosition(x,y);		
		fire.setPosition(x-(default_rocket_height*default_scale/2)*sin(r*PI/180),y+(default_rocket_height*default_scale/2)*cos(r*PI/180));
		fire_L.setPosition(x-(default_rocket_width*default_scale/2)*cos(r*PI/180)+(default_rocket_height*default_scale*3/8)*sin(r*PI/180),y-(default_rocket_height*default_scale*3/8)*cos(r*PI/180)-(default_rocket_width*default_scale/2)*sin(r*PI/180));
		fire_R.setPosition(x+(default_rocket_width*default_scale/2)*cos(r*PI/180)+(default_rocket_height*default_scale*3/8)*sin(r*PI/180),y-(default_rocket_height*default_scale*3/8)*cos(r*PI/180)+(default_rocket_width*default_scale/2)*sin(r*PI/180));
		rocket.setRotation(r);
		fire.setRotation(r);
		fire_L.setRotation(r+90);
		fire_R.setRotation(r-90);
		if(time_of_thrust>2)
			fire.setScale(1.5*fire_w_coef,3.5*fire_h_coef);
		else
			fire.setScale((0.5+time_of_thrust*0.5)*fire_w_coef,(0.5+time_of_thrust*1.5)*fire_h_coef);
		if(time_of_thrust==0)
			fire.setScale(0,0);
		if(time_rotating_left>2)
			fire_R.setScale(0.5*fire_w_coef,1.5*fire_h_coef);
		else
			fire_R.setScale((0.2+time_rotating_left*0.15)*fire_w_coef,(0.2+time_rotating_left*0.65)*fire_h_coef);
		if(time_rotating_left==0)
			fire_R.setScale(0,0);
		if(time_rotating_right>2)
			fire_L.setScale(0.5*fire_w_coef,1.5*fire_h_coef);
		else
			fire_L.setScale((0.2+time_rotating_right*0.15)*fire_w_coef,(0.2+time_rotating_right*0.65)*fire_h_coef);
		if(time_rotating_right==0)
			fire_L.setScale(0,0);
		if(dy>velocity_of_destruction*fixed_time && onGround)
			destroyed=true;
		vY=dy/fixed_time/default_scale*3.6;
		prewdy=dy/fixed_time;
	}
	void draw(RenderWindow& w){
		w.draw(rocket);
		w.draw(fire_L);
		w.draw(fire_R);
		w.draw(fire);
	}
	void ClearLine(COORD& pos,HANDLE& hout,int n){
		SetConsoleCursorPosition(hout,pos);
		for(;n>0;n--)
			cout<<' ';
		SetConsoleCursorPosition(hout,pos);
	}
	void ShowRocketStats(float& time){
		COORD pos={0,0};
		CONSOLE_CURSOR_INFO Curs;
		HANDLE hout=GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleCursorInfo(hout,&Curs);
		if(!stats_template_is_drawn){
			system("cls");
			cout<<"Time=\nX=\nY=\nR=\ndx=\ndy=\ndr=\nFalling:\nOnGround:\nDestroyed:\nThrust=\nTime Falling=\nVelocity Y=\n";
			stats_template_is_drawn=true;
		}else{
			Curs.bVisible=false;
			SetConsoleCursorInfo(hout,&Curs);
			pos={5,0};
			ClearLine(pos,hout,20);
			cout<<(int)time;
			pos={2,1};
			ClearLine(pos,hout,20);
			cout<<x;
			pos={2,2};
			ClearLine(pos,hout,20);
			cout<<y;
			pos={2,3};
			ClearLine(pos,hout,20);
			cout<<r;
			pos={3,4};
			ClearLine(pos,hout,20);
			cout<<dx;
			pos={3,5};
			ClearLine(pos,hout,20);
			cout<<dy;
			pos={3,6};
			ClearLine(pos,hout,20);
			cout<<dr;
			pos={8,7};
			ClearLine(pos,hout,20);
			cout<<falling;
			pos={9,8};
			ClearLine(pos,hout,20);
			cout<<onGround;
			pos={10,9};
			ClearLine(pos,hout,20);
			cout<<destroyed;
			pos={7,10};
			ClearLine(pos,hout,20);
			cout<<time_of_thrust;
			pos={13,11};
			ClearLine(pos,hout,20);
			cout<<time_falling_down;		
			pos={11,12};
			ClearLine(pos,hout,20);
			cout<<vY<<"km/h";
			Curs.bVisible=true;
			SetConsoleCursorInfo(hout,&Curs);
		}
	}
};
int main(){
	RenderWindow window(VideoMode(400,400),"SFML works!",Style::Fullscreen),*pw;
	Rocket rocket(4); //4 pixels = 1 meter
	Clock clock,cpu;
	pw=&window;
	while(window.isOpen()){
		float fixed_time,timer,zoom=1;
		fixed_time=cpu.getElapsedTime().asMicroseconds()/1000000.0;
		timer=clock.getElapsedTime().asMicroseconds()/1000000.0;
		cpu.restart();
		Event event;
		while(window.pollEvent(event)){
			if(event.type==Event::Closed)
				window.close();
		}
		rocket.update(fixed_time,zoom);
		window.clear();
		rocket.draw(window);
		window.display();
		rocket.ShowRocketStats(timer);
	}
	return 0;
}