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

for size in 1 2 4 16 32 64 128 256 512 1024; do
	# with threads from 1 to 4
	for th in {1..4}; do
		# Execution with 256i and 256r
		$prog $th 256 256 $size "K"
	done
	for th in {1..4}; do
		# 512i and 512r
		$prog $th 512 512 $size "K"
	done
	for th in {1..4}; do
		# 1024i and 1024r
		$prog $th 1024 1024 $size "K"
	done
	for th in {1..4}; do
		# 2048i and 2048r
		$prog $th 2048 2048 $size "K"
	done
done

# Executions with 512i and 256r, from 1 to 4 threads
# 2MB
for th in {1..4}; do
	$prog $th 512 256 2 "M"
done
# 4MB
for th in {1..4}; do
	$prog $th 512 256 4 "M"
done
# 8MB
for th in {1..4}; do
	$prog $th 512 256 8 "M"
done
# 16KB
for th in {1..4}; do
	$prog $th 512 256 16 "M"
done
# 32MB
for th in {1..4}; do
	$prog $th 512 256 32 "M"
done


if [[ $1 == "-a" || $1 == "-fi" ]]; then

	# This time we have 64 insertions and no reads
	for th in {1..4}; do
		# 512KB
		$prog $th 64 0 512 "K"
	done
	
	# The rest of the sizes
	for size in 0 1 2 3 4 5 6; do
		for th in {1..4}; do
			$prog $th 64 0 $((2**$size)) "M"
		done
	done
fi
