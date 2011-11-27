numNodes[0]=32
numNodes[1]=32
numNodes[2]=10
j=0
gcc DjikstrasFull.c -o djikstra
echo "" > task1.out
for  fileName in  `ls *.txt`
 do
		echo ${numNodes[$j]}
		
		for ((i=1;i<=${numNodes[$j]};i++ ))	
		do
			echo "File : $fileName"
			./djikstra $fileName $i $1 >> task1.out
		done
	j=$j+1
done
cat task1.out | grep "Source Node" | cut -d " " -f 4,9
