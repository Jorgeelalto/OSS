#############################################################
# Copy this script to the folder where the executables are, #
# otherwise it will not work                                #
#############################################################


# If the number of arguments passed is not 2, show help
if [ $# -ne 1 ]; then
	echo "Usage: ./tests.sh [PROGRAM]"
fi
prog=$1


echo "Number of threads, Number of insertions, Number of reads, Block Size, Size unit, Memory consumption, Total number of requests, Time elapsed, Requests per second"

for th in 1 4; do

	# Stage 1
	for size in 1 2 4 8 16 32 64 128 256 512 1024; do
		$prog $th 4096 4096 $size "K"
	done

	# Stage 2
	for size in 1 2 4 8 16 32 64 128 256 512; do
		$prog $th $(( 2048/size )) $(( 2048/size )) $size "M"
	done
done

# Stage 3	
for size in 1024 2048 4096; do
	$prog 2 $(( 16384/size )) $(( 16384/size )) $size "M"
done
