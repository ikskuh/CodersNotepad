using GLib;

class Program : Object
{
	int min;
	int max;
	public Program(int min, int max)
	{
		this.min = min;
		this.max = max;
	}

	public void print()
	{
		for(int i = this.min; i <= this.max; i++) {
			stdout.printf("%d * 2 = %d\n", i, 2*i);
		}
	}
}

int main(string[] args)
{
	print("Hello World\n");
	var pgm = new Program(10, 20);
	pgm.print();
	return 0;
}