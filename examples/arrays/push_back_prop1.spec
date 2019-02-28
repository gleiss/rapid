// similar to copy, but keeps track of the size of a and asserts the property with respect to the size of a

func main()
{
	Int[] a;
	const Int[] b;
	const Int blength;

	Int alength = 0;
	Int i = 0;
	while(i < blength)
	{
		a[i] = b[i];
		alength = alength + 1;
		i = i + 1;
	}
}

(assert-not
	(=>
		(<= 0 blength)
		(= (alength main_end) blength)
	)
)
