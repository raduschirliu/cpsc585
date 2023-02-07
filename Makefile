.PHONY: format

format:
	find game/src/ -iname *.h -o -iname *.cpp | xargs clang-format -i