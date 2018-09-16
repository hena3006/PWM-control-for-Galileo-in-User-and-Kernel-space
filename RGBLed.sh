#! /bin/bash

linuxio=( 11 12 13 14 6 0 1 38 40 4 10 5 15 7 )
levelshifter=( 32 28 34 16 36 18 20 0 0 22 26 24 42 30 )
mux=( 0 45 77 76 0 66 68 0 0 70 74 44 0 46 )

DIR="out"
ON=1
OFF=0

echo Enter Pin1
read pin1
gpio1[0]=${linuxio[pin1]}
gpio1[1]=${levelshifter[pin1]}
gpio1[2]=${mux[pin1]}

echo Enter Pin2
read pin2
gpio2[0]=${linuxio[pin2]}
gpio2[1]=${levelshifter[pin2]}
gpio2[2]=${mux[pin2]}

echo Enter Pin3
read pin3
gpio3[0]=${linuxio[pin3]}
gpio3[1]=${levelshifter[pin3]}
gpio3[2]=${mux[pin3]}

echo Enter value of PWM
read pwm
echo $pwm
cycleduration=20000
ton=$(((cycleduration*pwm)/100))
toff=$((cycleduration-ton))
echo $ton
echo $toff

#setup pin1
echo -n ${gpio1[0]} > /sys/class/gpio/export
for i in `seq 1 2`
do
	if [ ${gpio1[$i]} != 0 ]; then
		echo -n ${gpio1[$i]} > /sys/class/gpio/export
	fi
done

echo -n $DIR > /sys/class/gpio/gpio${gpio1[0]}/direction
if [ ${gpio1[1]} != 0 ]; then
	echo -n $DIR > /sys/class/gpio/gpio${gpio1[1]}/direction
fi

if [ ${gpio1[2]} < 66 || ${gpio1[2]} > 79 || ${gpio1[2]} != 0 ]; then
	echo -n $DIR > /sys/class/gpio/gpio${gpio1[2]}/direction
fi

for i in `seq 1 2`
do
	if [ ${gpio1[$i]} != 0 ]; then
		echo -n $OFF > /sys/class/gpio/gpio${gpio1[$i]}/value	
	fi
done

#setup pin2
echo -n ${gpio2[0]} > /sys/class/gpio/export
for i in `seq 1 2`
do
	if [ ${gpio2[$i]} != 0 ]; then
		echo -n ${gpio2[$i]} > /sys/class/gpio/export
	fi
done

echo -n $DIR > /sys/class/gpio/gpio${gpio2[0]}/direction
if [ ${gpio2[1]} != 0 ]; then
	echo -n $DIR > /sys/class/gpio/gpio${gpio2[1]}/direction
fi

if [ ${gpio2[2]} < 66 || ${gpio2[2]} > 79 || ${gpio2[2]} != 0 ]; then
	echo -n $DIR > /sys/class/gpio/gpio${gpio2[2]}/direction
fi

for i in `seq 1 2`
do
	if [ ${gpio2[$i]} != 0 ]; then
		echo -n $OFF > /sys/class/gpio/gpio${gpio2[$i]}/value	
	fi
done

#setup pin3
echo -n ${gpio3[0]} > /sys/class/gpio/export
for i in `seq 1 2`
do
	if [ ${gpio3[$i]} != 0 ]; then
		echo -n ${gpio3[$i]} > /sys/class/gpio/export
	fi
done

echo -n $DIR > /sys/class/gpio/gpio${gpio3[0]}/direction
if [ ${gpio3[1]} != 0 ]; then
	echo -n $DIR > /sys/class/gpio/gpio${gpio3[1]}/direction
fi

if [ ${gpio3[2]} < 66 || ${gpio3[2]} > 79 || ${gpio3[2]} != 0 ]; then
	echo -n $DIR > /sys/class/gpio/gpio${gpio3[2]}/direction
fi

for i in `seq 1 2`
do
	if [ ${gpio3[$i]} != 0 ]; then
		echo -n $OFF > /sys/class/gpio/gpio${gpio3[$i]}/value	
	fi
done
#setup complete at this point

for i in `seq 1 25`
do
	echo -n $ON > /sys/class/gpio/gpio${gpio1[0]}/value
	usleep $ton	
	echo -n $OFF > /sys/class/gpio/gpio${gpio1[0]}/value
	usleep $toff
done

for i in `seq 1 25`
do
	echo -n $ON > /sys/class/gpio/gpio${gpio2[0]}/value
	usleep $ton	
	echo -n $OFF > /sys/class/gpio/gpio${gpio2[0]}/value
	usleep $toff
done

for i in `seq 1 25`
do
	echo -n $ON > /sys/class/gpio/gpio${gpio3[0]}/value
	usleep $ton	
	echo -n $OFF > /sys/class/gpio/gpio${gpio3[0]}/value
	usleep $toff
done

for i in `seq 1 25`
do
	echo -n $ON > /sys/class/gpio/gpio${gpio1[0]}/value
	echo -n $ON > /sys/class/gpio/gpio${gpio2[0]}/value
	usleep $ton	
	echo -n $OFF > /sys/class/gpio/gpio${gpio1[0]}/value	
	echo -n $OFF > /sys/class/gpio/gpio${gpio2[0]}/value
	usleep $toff
done

for i in `seq 1 25`
do
	echo -n $ON > /sys/class/gpio/gpio${gpio2[0]}/value
	echo -n $ON > /sys/class/gpio/gpio${gpio3[0]}/value
	usleep $ton	
	echo -n $OFF > /sys/class/gpio/gpio${gpio2[0]}/value	
	echo -n $OFF > /sys/class/gpio/gpio${gpio3[0]}/value
	usleep $toff
done

for i in `seq 1 25`
do
	echo -n $ON > /sys/class/gpio/gpio${gpio3[0]}/value
	echo -n $ON > /sys/class/gpio/gpio${gpio1[0]}/value
	usleep $ton	
	echo -n $OFF > /sys/class/gpio/gpio${gpio3[0]}/value	
	echo -n $OFF > /sys/class/gpio/gpio${gpio1[0]}/value
	usleep $toff
done

for i in `seq 1 25`
do
	echo -n $ON > /sys/class/gpio/gpio${gpio1[0]}/value	
	echo -n $ON > /sys/class/gpio/gpio${gpio2[0]}/value
	echo -n $ON > /sys/class/gpio/gpio${gpio3[0]}/value
	usleep $ton	
	echo -n $OFF > /sys/class/gpio/gpio${gpio1[0]}/value	
	echo -n $OFF > /sys/class/gpio/gpio${gpio2[0]}/value
	echo -n $OFF > /sys/class/gpio/gpio${gpio3[0]}/value
	usleep $toff
done

echo -n ${gpio1[0]} > /sys/class/gpio/unexport
echo -n ${gpio1[1]} > /sys/class/gpio/unexport
echo -n ${gpio1[2]} > /sys/class/gpio/unexport

echo -n ${gpio2[0]} > /sys/class/gpio/unexport
echo -n ${gpio2[1]} > /sys/class/gpio/unexport
echo -n ${gpio2[2]} > /sys/class/gpio/unexport

echo -n ${gpio3[0]} > /sys/class/gpio/unexport
echo -n ${gpio3[1]} > /sys/class/gpio/unexport
echo -n ${gpio3[2]} > /sys/class/gpio/unexport

