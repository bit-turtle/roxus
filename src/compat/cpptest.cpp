
class Test {
	private:
	int a = 0;
	public:
	Test(int b = 0x4c8) {
		a = b;
	}
	int geta() {
		return a;
	}
};

extern "C" {
	int cpptest() {
		Test* a = new Test();
		int b = a->geta();
		delete a;
		return b;
	}
}
