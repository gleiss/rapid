func main()
{
	Int[] a;
	const Int[] b;
	const Int blength;

	Int i = 0;
	Int alength = 0;
	while(i < blength)
	{
		if (b[i] >= 0)
		{
			a[alength] = b[i];
			alength = alength + 1;
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
				(< k (alength main_end))
				(<= 0 blength)
			)
			(<= 0 (a main_end k))
		)
	)
)

(conjecture
	(forall ((k Int))
		(exists ((l Int))
			(=>
				(and
					(<= 0 k)
					(< k (alength main_end))
					(<= 0 blength)
				)
				(= (a main_end k) (b l))
			)
		)
	)
)