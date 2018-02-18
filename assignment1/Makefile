all: Ex5 Ex7 Ex8 Ex9

test: Ex5_test Ex7_test Ex8_test Ex9_test

Ex5:
	python3 ../convertAddr.py Ex5.txt Ex5_compile.txt

Ex5_test:
	python3 ../convertAddr.py Ex5.txt Ex5_compile.txt -t 1.5_b15_extract.txt

Ex7:
	python3 ../convertAddr.py Ex7.txt Ex7_compile.txt

Ex7_test:
	python3 ../convertAddr.py Ex7.txt Ex7_compile.txt -t 1.7_16b_sum_carry.txt

Ex8:
	python3 ../convertAddr.py Ex8.txt Ex8_compile.txt

Ex8_test:
	python3 ../convertAddr.py Ex8.txt Ex8_compile.txt -t 1.8_32b_sum.txt

Ex9:
	python3 ../convertAddr.py Ex9.txt Ex9_compile.txt

Ex9_test:
	python3 ../convertAddr.py Ex9.txt Ex9_compile.txt -t 1.9_mul.txt

clean:
	rm Ex*_compile.txt