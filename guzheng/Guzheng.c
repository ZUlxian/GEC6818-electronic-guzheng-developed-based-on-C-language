#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <stdbool.h>
#include <stdlib.h>	
#include <signal.h>	
#include <sys/wait.h>

/* ********************** 宏定义 ********************** */
#define Pathname_Lcd 	"/dev/fb0"				// GEC6818显示屏lcd文件路径
#define Pathname_Touch 	"/dev/input/event0"		// GEC6818触摸屏驱动
//#define Pathname_Lcd 	"/dev/ubuntu_lcd"			// Ubuntu模拟屏lcd文件路径
//#define Pathname_Touch 	"/dev/ubuntu_event"			// 模拟屏触摸驱动

/* ******************* 全局变量定义 ******************* */
int fd_lcd = -1;
int *p = NULL;
pid_t madplay_pid = -1;
int current_music_index = 0;

/* ********************* 函数声明 ********************* */
int Device_Init(void);
void Device_Uninit(void);
int Touch_coordinate(int *ts_x, int *ts_y);
int Show_bmp(int bmp_x, int bmp_y, char *Pathname_bmp);
void Mmap_rectangle(int x_start, int x_end, int y_start, int y_end, int color);
void Mmap_circle(int circle_x, int circle_y, int radius, int color);
void Draw_pixel(int x, int y, int pixel);
int Touch_control();
void Music_play();
void Guzheng();
void stopMadplay(void);

/* ********************** 主函数 ********************** */
int main(int argc, char **argv)
{
	int flag_system = 1;
	int x, y;
	
	Device_Init();
	
	Show_bmp(0, 0, "Desktop.bmp");
	printf("开始程序\n");
	
	Touch_coordinate(&x, &y);
	
	while(1)
	{
		printf("------------\n");
		Show_bmp(0, 0, "Interface.bmp");
		printf("主界面\n");
		Touch_control();
	}
	
	Device_Uninit();

	return 0;
}

/* ********************* 逻辑函数 ********************* */
// 触摸屏控制主界面函数
int Touch_control()
{
	int x, y;
	printf("_LINE_%d\n",65);
	Touch_coordinate(&x, &y);
	printf("(%d,%d)\n", x, y);
	
	
	if(x>0 && x<400 && y>0 && y<480)
	{
		printf("Music play：");
		Music_play();
	}
	
	if(x>400 && x<800  && y>0 && y<480)
	{
		printf("Guzheng：");
		Guzheng();
		
	}
	
}


// madplay播放音频
void Music_play()
{
	char *bmp_buf[] = {"1.bmp", "2.bmp", "3.bmp", "4.bmp"};
	const char *music_files[] = {"Gujian3_music1.mp3","Gujian3_music2.mp3","Gujian3_music3.mp3","Gujian3_music4.mp3",};
    int x, y;
	int i = 0;
	int madplay_code = 0;
	int madplay_flag = 0;
	
	printf("madplay功能\n");
	
	Show_bmp(0, 80, "1.bmp");
	Show_bmp(0, 0, "Madplay_Stop_Interface.bmp");

    while (1) {
		Show_bmp(0, 80, bmp_buf[i]);		// 显示第x张图片
		
        Touch_coordinate(&x, &y);
        printf("(%d,%d)\n", x, y);
		
			if (x >= 0 && x < 200 && y >= 400 && y < 480) {
			printf("功能3：切换上一首\n");
			i++;
			if(i > 3)i=0;
			// 停止当前音乐
			system("killall -9 madplay");

			// 切换到上一首音乐
			current_music_index = (current_music_index - 1 + sizeof(music_files) / sizeof(music_files[0])) % (sizeof(music_files) / sizeof(music_files[0]));

			// 播放上一首音乐
			madplay_pid = fork();
			if (madplay_pid == -1) {
				perror("fork");
			} else if (madplay_pid == 0) {
				char filename[20];
				sprintf(filename, "%s", music_files[current_music_index]);
				execlp("madplay", "madplay", filename, (char *)NULL);
				perror("execlp");
				exit(EXIT_FAILURE);
			} 
			else
				{
					Show_bmp(0, 0, "Madplay_Cont_Interface.bmp");
					system("killall -CONT madplay");
					printf("继续\n");
				}
			
        }
        
		if(x>=200 && x<400 && y>=400 && y<480)
		{
			printf("功能2：");
			if(madplay_code == 0)
			{
				madplay_code = 1;
				if(madplay_flag == 0)
				{
					Show_bmp(0, 0, "Madplay_Cont_Interface.bmp");
					madplay_flag = 1;		// 音乐是否第一次运行
					system("madplay Gujian3_music1.mp3 &");	// 播放音乐
					printf("开始播放\n");
				}
				else
				{
					Show_bmp(0, 0, "Madplay_Cont_Interface.bmp");
					system("killall -CONT madplay");
					printf("继续\n");
				}		
			}	
			else
			{
				Show_bmp(0, 0, "Madplay_Stop_Interface.bmp");
				madplay_code = 0;
				system("killall -STOP madplay");
				printf("暂停\n");
			}			
		}
        
		if (x >= 400 && x < 600 && y >= 400 && y < 480) {
			printf("功能4：切换下一首\n");
			i--;
			if(i < 0)i=3;
			// 停止当前音乐
			system("killall -9 madplay");

			// 切换到下一首音乐
			current_music_index = (current_music_index + 1) % (sizeof(music_files) / sizeof(music_files[0]));

			// 播放下一首音乐
			madplay_pid = fork();
			if (madplay_pid == -1) {
				perror("fork");
			} else if (madplay_pid == 0) {
				char filename[20];
				sprintf(filename, "%s", music_files[current_music_index]);
				execlp("madplay", "madplay", filename, (char *)NULL);
				perror("execlp");
				exit(EXIT_FAILURE);
			} 
			else
				{
					Show_bmp(0, 0, "Madplay_Cont_Interface.bmp");
					system("killall -CONT madplay");
					printf("继续\n");
				}
			
        }

        if (x >= 700 && x < 800 && y >= 400 && y < 480) 
		{
			printf("功能4：退出");
			system("killall -9 madplay");
			break;
		}
    }
}

//古筝
void stopMadplay() {
    if (madplay_pid != -1) {
        kill(madplay_pid, SIGKILL);  // 使用 SIGKILL 信号强制终止进程
        madplay_pid = -1;
    }
}
// Guzheng
void Guzheng() {
    int x, y;
	int madplay_code = 0;
	int madplay_flag = 0;
	int noteIndex = -1; 
	
	printf("Guzheng功能\nKey=D\n");
	
	Show_bmp(0, 0, "Wood.bmp");
	Show_bmp(0, 50, "Green_Line.bmp");
	Show_bmp(0, 100, "Line.bmp");
	Show_bmp(0, 150, "Line.bmp");
	Show_bmp(0, 200, "Line.bmp");
	Show_bmp(0, 250, "Line.bmp");
	Show_bmp(0, 300, "Green_Line.bmp");
	Show_bmp(0, 350, "Line.bmp");
	Show_bmp(0, 400, "Line.bmp");
	Show_bmp(0, 450, "Line.bmp");
   


    while (1) {
        Touch_coordinate(&x, &y);
        printf("(%d,%d)\n", x, y);

        // 根据触摸坐标确定音符
        if(x>=0 && x<800 && y>=20 && y<50) noteIndex = 0;
        else if (x>=0 && x<800 && y >= 70 && y < 100) noteIndex = 1;
        else if (x>=0 && x<800 && y >= 120 && y < 150) noteIndex = 2;
        else if (x>=0 && x<800 && y >= 170 && y < 200) noteIndex = 3;
        else if (x>=0 && x<800 && y >= 220 && y < 250) noteIndex = 4;
        else if (x>=0 && x<800 && y >= 270 && y < 300) noteIndex = 5;
        else if (x>=0 && x<800 && y >= 320 && y < 350) noteIndex = 6;
        else if (x>=0 && x<800 && y >= 370 && y < 400) noteIndex = 7;
        else if (x>=0 && x<800 && y >= 420 && y < 450) noteIndex = 8;

        if (noteIndex != -1) {
            printf("功能%d：", noteIndex);

            // 在启动新进程之前停止之前的 madplay 进程
            stopMadplay();

            // fork 一个子进程来播放音频
            madplay_pid = fork();

            if (madplay_pid == -1) {
                perror("fork");
            } else if (madplay_pid == 0) {
                // 子进程
                char filename[20];
                sprintf(filename, "%d.mp3", noteIndex);
                execlp("madplay", "madplay", filename, (char *)NULL);
                perror("execlp");
                exit(EXIT_FAILURE);
            } else {
				
                
            }
        }


        if(x>=700 && x<800 && y>=450 && y<480){
            printf("功能10：退出");
            // 在退出之前停止 madplay 进程
            stopMadplay();
            break;
        }
    }
}


/* ********************* 函数定义 ********************* */
/* 初始化硬件 */
int Device_Init(void)
{
	// 打开lcd驱动文件：
	fd_lcd = open(Pathname_Lcd, O_RDWR);
	if(fd_lcd == -1)
	{
		perror("open lcd failed");
		return -1;
	}
	
	// 映射lcd驱动
	p = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED, fd_lcd, 0);
	if(p == NULL)
	{
		perror("mmap lcd failed");
		return -1;
	}
}

/* 解除硬件初始化 */
void Device_Uninit(void)
{
	// 解除映射
	munmap(p, 800*480*4);
	
	// 关闭lcd驱动文件
	close(fd_lcd);	
}

/* 映射显示bmp格式图片：*/
int Show_bmp(int bmp_x, int bmp_y, char *Pathname_bmp)
{
	int fd_bmp = -1;
	int num_pixel = 0;
	int width_bmp, height_bmp;
	unsigned char R, G, B;
	int pixel_one = 0;
	
	// 打开图片文件
	fd_bmp = open(Pathname_bmp, O_RDWR);
	if(fd_bmp == -1)
	{
		printf("open %s failed", Pathname_bmp);
		return -1;
	}
	
	// 获取图片高度和宽度：确定储存像素点数据的数组大小
	lseek(fd_bmp, 18, SEEK_SET);
	read(fd_bmp, &width_bmp, sizeof(int));
	read(fd_bmp, &height_bmp, sizeof(int));
	char pixel_bmp[width_bmp*height_bmp*3];
	if((bmp_y+height_bmp)>480 || (bmp_x+width_bmp)>800)
		printf("BMP out of Lcd range\n");
	
	// 跳过BMP图片前54位图片信息数据，并获取颜色深度数据
	lseek(fd_bmp, 54, SEEK_SET);
	read(fd_bmp, pixel_bmp, sizeof(pixel_bmp));
	
	// 图片颜色深度顺序处理，并映射处理后的颜色深度数据到lcd中
	for(int y=bmp_y; y<(bmp_y+height_bmp); y++)
	{
		for(int x=bmp_x; x<(bmp_x+width_bmp); x++)
		{
			// B G R -> RGB
			B = pixel_bmp[num_pixel++];
			G = pixel_bmp[num_pixel++];
			R = pixel_bmp[num_pixel++];		
			pixel_one = (R<<16|G<<8|B<<0);
			
			// 映射颜色深度数据到lcd中。(479-y)：图片像素上下翻转
			Draw_pixel(x, (479-y), pixel_one);
		}
	}
}

/* lcd映射颜色深度 */
void Draw_pixel(int x, int y, int pixel)
{	
	// 限制x、y值不能超出lcd显示屏范围
	if(x>800 || x<0 || y>480 || y<0)
		printf("pixel out of Lcd range\n");
	*(p+x+y*800) = pixel;
}

/* 获取触摸屏坐标 */
int Touch_coordinate(int *ts_x, int *ts_y)
{
	int fd_ts = -1;
	
	// 打开触摸屏驱动文件
	fd_ts = open(Pathname_Touch, O_RDWR);
	if(fd_ts == -1)
	{
		perror("open touch_event failed");
		return -1;
	}
	
	struct input_event touch_buf;		// 定义输入子系统结构体
	// int ts_x = -1;
	// int ts_y = -1;
	int flag_x = false;
	int flag_y = false;
	
	while(1)
	{
		// 1.2 读取驱动文件数据，保存到输入子系统中
		read(fd_ts, &touch_buf, sizeof(touch_buf));
		printf("_LINE_%d\n",595);
		if(touch_buf.type==EV_ABS && touch_buf.code==ABS_X)
		{
			*ts_x = touch_buf.value;	// 保存x轴坐标
			flag_x = true;				// x轴坐标标志位：当有数值时，标志位为真
		}
		if(touch_buf.type==EV_ABS && touch_buf.code==ABS_Y)
		{
			*ts_y = touch_buf.value;	// 保存y轴坐标
			flag_y = true;				// y轴坐标标志位：当有数值时，标志位为真
		}
		if(flag_x && flag_y)
		{
			flag_x = false;		// 清空x轴标志位
			flag_y = false;		// 清空y轴标志位
			break;
		}
	}
	
	// printf("(%d,%d)\n", ts_x, ts_y);

	close(fd_ts);	// 关闭lcd驱动文件
}

/* 映射显示长方形颜色 */
void Mmap_rectangle(int x_start, int x_end, int y_start, int y_end, int color)
{
	for(int y=y_start; y<y_end; y++)
	{
		for(int x=x_start; x<x_end; x++)
		{
			*(p+x+y*800) = color;
		}
	}
}

/* 映射显示圆形
	圆形公式：	x²+y²=r²
	circle_x：	圆心x坐标
	circle_y：	圆心y坐标
	radius：	圆半径
*/
void Mmap_circle(int circle_x, int circle_y, int radius, int color)
{
	for(int y=(circle_y-radius); y<(circle_y+radius); y++)
	{
		for(int x=(circle_x-radius); x<(circle_x+radius); x++)
		{
			if(((circle_x-x)*(circle_x-x) + (circle_y-y)*(circle_y-y)) < radius*radius)
			{
				*(p+x+y*800) = color;
			}
		}
	}
}
