all : touchpad.svg

touchpad_gen : touchpad_gen.c
	gcc -o $@ $^ -lm

touchpad.svg : touchpad_gen
	./touchpad_gen gitouchpad.svg

clean :
	rm -rf touchpad_gen touchpad.svg
