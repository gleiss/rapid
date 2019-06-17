func main()
{
	Int[] a;
	const Int[] b;

	const Int blength;

	Int i = 0;
	while(i < blength)
	{
		if(b[i] >= 0)
		{
			a[i] = b[i];
		}
		else
		{
			a[i] = 0-b[i];
		}
		i = i + 1;
	}
}

(conjecture
	(forall ((k Int))
		(=>
			(and
				(<= 0 k)
				(< k blength)
			)
			(or
				(= (a main_end k) (b k) )
				(= (a main_end k) (- 0 (b k)) )
			)
		)
	)
)

(conjecture
	(forall ((k Int))
		(=>
			(and
				(<= 0 k)
				(< k blength)
			)
				(<= 0 (a main_end k))
		)
	)
)
