struct numbers{
	int a;
	int b;
    string path<>;
};
program MY_PROG{
	version MY_VERS{
		string add(numbers)=1;
	}=1;
}=0x12345678;
