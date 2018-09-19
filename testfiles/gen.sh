if [ $1 == "-r" ]; then
	echo "Removing all the previously generated dummy files"
	rm *K* *M*
	exit 1
fi

if [ $1 == "-c" ]; then
	echo "Compiling gen binary"
	gcc gen.c -o gen
	exit 1
fi

# Compile gen
echo "Compiling gen binary"
gcc gen.c -o gen

# Generating special size files (2GB and 4GB)
echo "Generating special size files"
./gen 2048 M 2048Ma a
./gen 4096 M 4096Ma a

# For both MB and KB
for l in K M; do
	# And taking all sizes in powers of two between 1 and 1024
	for s in 1 2 4 8 16 32 64 128 256 512 1024; do
		# If its going to generate files bigger than 64MB, stop
		#if [ $l == 'M' ] && [ $s -gt 64 ]; then
		#	exit 1
		#fi
		# Generate the files with 4 different letters
		for c in a; do
			./gen $s $l $s$l$c $c
		done
	done
done
