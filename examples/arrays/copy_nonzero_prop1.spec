func main()
{
	Int[] a;

	const Int[] b;
	const Int blength;

	Int i = 0;
	Int alength = 0;
	while(i < blength)
	{
		if (b[i] != 0)
		{
			a[alength] = b[i];
			alength = alength + 1;
		}
		else
		{
			skip;
		}
		i = i + 1;
	}
}

(conjecture
	(forall ((k Int))
		(=>
			(and
				(<= 0 blength)
				(<= 0 k)
				(< k (alength main_end))
			)
			(not (= (a main_end k) 0))
		)
	)
)
