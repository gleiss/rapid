func main()
{
	Int[] a;
	const Int[] b;
	const Int blength;

	Int i = 0;
	Int j = 0;
	while(i < blength)
	{
		if (b[i] >= 0)
		{
			a[j] = b[i];
			j = j + 1;
		} else
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
				(<= 0 k)
				(< k (j main_end))
				(<= 0 blength)
			)
			(<= 0 (a main_end k))
		)
	)
)
