# If the number of arguments passed is not 2, show help
if [ $# -ne 2 ]; then
	echo "Usage: ./tests.sh [TYPE] [PROGRAM]"
	echo "Test types:"
	echo "	-a: run all"
	echo "	-m: run mixed IO only"
	echo "	-fi: run fast insertions only"
	echo "	-hi: run heavy insertions only"
	echo "	-fr: run fast reads only"
	echo "	-hr: run heavy reads only"
fi


prog=$2

# Run the mixed IO tests
if [[ $1 == "-a" || $1 == "-m" ]]; then
	echo "======================"
	echo "Running mixed IO tests"
	echo "======================"

	# with all the MB files
	for size in 128 256 512; do
		# with threads from 1 to 4
		for th in {1..4}; do
			echo ID: 01 $prog $th 128 128 $size M
			$prog $th 128 128 $size 'M'
		done
		for th in {1..4}; do
			echo ID: 02 $prog $th 256 256 $size M
			$prog $th 256 256 $size 'M'
		done
		for th in {1..4}; do
			echo ID: 03 $prog $th 512 512 $size M
			$prog $th 512 512 $size 'M'
		done
	done
fi


if [[ $1 == "-a" || $1 == "-fi" ]]; then
	echo "============================"
	echo "Running fast insertion tests"
	echo "============================"

	# This time we have various insertions of 128MB
	for th in {1..4}; do
		for i in {1..10}; do
			echo ID: $((3 + $i)) $prog $th $((50 * $i)) 0 128 M
			$prog $th $((50 * i)) 0 128 M
		done
	done
fi


if [[ $1 == "-a" || $1 == "-hi" ]]; then
	echo "============================="
	echo "Running heavy insertion tests"
	echo "============================="
	
	# 1GB
	for th in {1..4}; do
		echo ID: 14 $prog $th 16 0 1024 M
		$prog $th 16 0 1024 M
	done
	# 2GB
	for th in {1..4}; do
		echo ID: 15 $prog $th 16 0 2048 M
		$prog $th 16 0 2048 M
	done
	# 4GB
	for th in {1..4}; do
		echo ID: 16 $prog $th 16 0 4096 M
		$prog $th 16 0 4096 M
	done
fi


if [[ $1 == "-a" || $1 == "-fr" ]]; then
	echo "========================"
	echo "Running fast reads tests"
	echo "========================"

	echo "Inserting data before running read test"
	$prog 1 0 500 128 'M'

	# This time we have various reads of 128MB
	for th in {1..4}; do
		for i in {1..10}; do
			echo ID: $((16 + i)) $prog $th 0 $((50 * i)) 128 M
			$prog $th 0 $((50 * i)) 128 M
		done
	done
fi


if [[ $1 == "-a" || $1 == "-hr" ]]; then
	echo "========================="
	echo "Running heavy reads tests"
	echo "========================="

	echo "Inserting data before running read test"
	$prog 1 16 0 4096 'M'
	
	# 1GB
	for th in {1..4}; do
		echo ID: 27 $prog $th 0 16 1024 M
		$prog $th 16 0 1024 M
	done
	# 2GB
	for th in {1..4}; do
		echo ID: 28 $prog $th 0 16 2048 M
		$prog $th 16 0 2048 M
	done
	# 4GB
	for th in {1..4}; do
		echo ID: 29 $prog $th 0 16 4096 M
		$prog $th 16 0 4096 M
	done
fi
