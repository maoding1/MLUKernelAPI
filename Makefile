INCLUDE_PATH = ./include
KERNEL_PATH = ./mluKernel
TESTFILE_PATH = ./test
OUTPUT_PATH = ./build
CNCC_FLAG = --bang-mlu-arch=mtp_372 -c -fPIE
CXX_TEST_FLAG = -std=c++11 -c -I $(INCLUDE_PATH)
CXX_LINK_FLAG = -L /usr/local/neuware/lib64 -lcnrt
relu:
	cncc $(KERNEL_PATH)/$@_kernel.mlu -o $(OUTPUT_PATH)/$@_kernel.o ${CNCC_FLAG}
	g++  $(TESTFILE_PATH)/$@_test.cpp $(CXX_TEST_FLAG) -o $(OUTPUT_PATH)/$@.o
	g++  $(OUTPUT_PATH)/$@_kernel.o $(OUTPUT_PATH)/$@.o -o $(OUTPUT_PATH)/$@_test $(CXX_LINK_FLAG)
	rm $(OUTPUT_PATH)/*.o
	$(OUTPUT_PATH)/$@_test

maxpool2d:
	cncc $(KERNEL_PATH)/$@_kernel.mlu -o $(OUTPUT_PATH)/$@_kernel.o ${CNCC_FLAG}
	g++  $(TESTFILE_PATH)/$@_test.cpp $(CXX_TEST_FLAG) -o $(OUTPUT_PATH)/$@.o
	g++  $(OUTPUT_PATH)/$@_kernel.o $(OUTPUT_PATH)/$@.o -o $(OUTPUT_PATH)/$@_test $(CXX_LINK_FLAG)
	rm $(OUTPUT_PATH)/*.o
	$(OUTPUT_PATH)/$@_test

avgpool2d:
	cncc $(KERNEL_PATH)/$@_kernel.mlu -o $(OUTPUT_PATH)/$@_kernel.o ${CNCC_FLAG}
	g++  $(TESTFILE_PATH)/$@_test.cpp $(CXX_TEST_FLAG) -o $(OUTPUT_PATH)/$@.o
	g++  $(OUTPUT_PATH)/$@_kernel.o $(OUTPUT_PATH)/$@.o -o $(OUTPUT_PATH)/$@_test $(CXX_LINK_FLAG)
	rm $(OUTPUT_PATH)/*.o
	$(OUTPUT_PATH)/$@_test

batchnorm2d:
	cncc $(KERNEL_PATH)/$@_kernel.mlu -o $(OUTPUT_PATH)/$@_kernel.o ${CNCC_FLAG}
	g++  $(TESTFILE_PATH)/$@_test.cpp $(CXX_TEST_FLAG) -o $(OUTPUT_PATH)/$@.o
	g++  $(OUTPUT_PATH)/$@_kernel.o $(OUTPUT_PATH)/$@.o -o $(OUTPUT_PATH)/$@_test $(CXX_LINK_FLAG)
	rm $(OUTPUT_PATH)/*.o
	$(OUTPUT_PATH)/$@_test

clean:
	rm $(OUTPUT_PATH)/*_test
