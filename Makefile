formatter: formatter/formatter compressor/output_compressed.lz4
	cd formatter && ./formatter

formatter/formatter: formatter/*.c formatter/*.h
	cd formatter && make

compressor/output_compressed.lz4: compressor/output_compressed
	yes | lz4 -12 -B10000 compressor/output_compressed compressor/output_compressed.lz4

compressor/output_compressed: optimizer/output/full compressor/compressor
	cd compressor && ./compressor

compressor/compressor: compressor/*.c compressor/*.h
	cd compressor && make

optimizer/output/full: optimizer/optimizer
	cd optimizer && ./optimizer

optimizer/optimizer: optimizer/*.c optimizer/*.h
	cd optimizer && make

render: compressor/output_compressed renderer/renderer
	cd renderer && ./renderer

renderer/renderer: renderer/*.c renderer/*.h
	cd renderer && make

