`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    21:14:54 09/17/2013 
// Design Name: 
// Module Name:    test2 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additioal Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module test1(
input clk,
input power_on,
input out1,  //外部电梯楼层按键
input out3,
input out6,
input out8,
input in1,  //内部电梯楼层按键
input in3,
input in6,
input in8,
input door_open,
input door_close,
output reg direct,  //方向表示，0表示UP，1表示down
output reg state,   //运行状态，闪烁则代表在运行，熄灭则代表停止
output reg light,
output reg floor1,
output reg floor3,
output reg floor6,
output reg floor8,
output reg opendoor,//开门状态 =1 则为开门
output reg [6:0] LEDdisplay
    );
reg [7:0] curFloor;  //当前电梯所在楼层，只能有一个1带里面
reg [7:0] insideFloor;  //电梯内部楼层按钮，允许多次出现1；
reg [23:0] cicleTime;
reg [2:0]run;  //电梯运行状态
reg [4:0]number;

initial
begin
direct = 0;
state = 0;
floor1 = 0;
floor3 = 0;
floor6 = 0;
floor8 = 0;
light = 0;
opendoor = 0;
curFloor = 8'b00000001;//初始状态在一楼
cicleTime = 0;
LEDdisplay = 7'b1111001;
insideFloor = 8'b00000000;
run = 0;
number = 0;
end
always@(posedge clk)
 begin
	if(power_on == 1)
	begin
		state = 1;
		if(run == 0)
		run = 4;
		if(out1|in1)  //防止在一楼的时候有人狂按一楼里外的两个按钮，临时开门
		begin
			floor1 = 1;
			insideFloor[0] = 1;
			run = 1;
		end
		else if(out3|in3)
			begin
				floor3 = 1;
				insideFloor[2] = 1;
				run = 1;
			end
		else if(out6|in6)
			begin
				floor6 = 1;
				insideFloor[5] = 1;
				run = 1;
			end
		else if(out8|in8)
			begin
				floor8 = 1;
				insideFloor[7] = 1;
				run = 1;
			end		
		case(run)
		1:
		begin
			opendoor = 0;
			if(cicleTime[23]==1)
			begin  //~
			light = light+1;
			cicleTime =  24'd0;
			number = number + 1;
			run = 1;
			if(number ==11)
			begin //!
			number = 0;
			cicleTime = 0;
			if(!(curFloor&insideFloor))
				begin
					if(insideFloor > curFloor)
						begin
							if(direct == 0)
							begin
							
								curFloor = curFloor << 1;
								run = 2;
							end
							else
							begin
								direct = 0;
							end
						end
					else if(insideFloor < curFloor)
						begin
							if(direct == 1)
							begin
								//light = 1;
								curFloor = curFloor >> 1;
								run = 2;
							end
							else
							begin
							direct = 1;
							end
						end
				end
				begin
					run = 3;
				end
				case(curFloor)
				8'b00000001:
					begin
					LEDdisplay = 7'b1111001;
					if(insideFloor[0]==1)
						begin
							insideFloor[0] = 0;
							floor1= 0;
							run = 2;
						end
					end
				8'b00000010:
					begin
					LEDdisplay = 7'b0100100;
					end
				8'b00000100:
					begin
					LEDdisplay = 7'b0110000;
					if(insideFloor[2]==1)
						begin
							insideFloor[2] = 0;
							floor3 = 0;
							run = 2;
						end
					end
				8'b00001000:
					begin
					LEDdisplay = 7'b0011001;
					end
				8'b00010000:
					begin
					LEDdisplay = 7'b0010010;
					end
				8'b00100000:
					begin
					LEDdisplay = 7'b0000010;
					if(insideFloor[5]==1)
						begin
							insideFloor[5] = 0;
							floor6 = 0;
							run = 2;
						end
					end
				8'b01000000:
					begin
					LEDdisplay = 7'b1111000;
					end
				8'b10000000:
					begin
					LEDdisplay = 7'b0000000 ;
					if(insideFloor[7]==1)
						begin
							insideFloor[7] = 0;
							floor8 = 0;
							run = 2;
						end
					end
				endcase
			end  //!
			end  //~
			else 
				begin
					cicleTime = cicleTime + 1;
					run = 1;
				end
		end
		
		2:
		begin
			opendoor = 1;
			if(cicleTime[23] == 1)
				begin
						cicleTime = 0;
						number = number +1;
					if(number == 24)
						begin
						opendoor = 0;
						run = 3;
						number = 0;
						end
					else
						begin
						run = 2;
						end
				end
			else
				begin
					cicleTime = cicleTime + 1;
				if(door_close == 1)
					begin
						opendoor = 0;
						run = 3;
					end
				if(door_open == 1)
					begin
						opendoor = 1;
						run = 2;
					end
				end
		end
		3:
		begin
			light = 1;
			if(insideFloor == 0)
				begin
					light =0;
					run = 4;
				end
			else
				begin
					run = 1;
				end
		end
		4:
		begin
			if(door_open == 1)
				opendoor = 1;
			if(door_close == 1)
				opendoor = 0;
		end
		endcase
		end
	else
			begin
			direct = 0;
			state = 0;
			floor1 = 0;
			floor3 = 0;
			floor6 = 0;
			floor8 = 0;
			light = 0;
			opendoor = 0;
			curFloor = 8'b00000001;//初始状态在一楼
			cicleTime = 0;
			LEDdisplay = 7'b1111001;
			insideFloor = 8'b00000000;
			run = 0;
			end

end


 endmodule
 