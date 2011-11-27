gcc DistanceVector.c -o distanceVector
numNodes[0]=32
numNodes[1]=32
numNodes[2]=10
j=0
echo "" > task2.out
for  fileName in  `ls *.txt`

 do
		#echo ${numNodes[$j]}
		
		for ((i=1;i<=${numNodes[$j]};i++ ))	
		do
			echo ""
			echo "File : $fileName"
			./distanceVector $i $fileName $i $1 >> task2.out
		done
	j=$j+1
done
cat task2.out | grep "Num nodes :" | cut -d " " -f 4,9,18
