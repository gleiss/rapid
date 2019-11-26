func main()
{
	const Int[] a;
	const Int alength;

	Int i = 0;
	Int min = 0;
	while (i < alength)
	{
		if (a[i] < min)
		{
			min = a[i];
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
				(<= 0 alength)
				(<= 0 k)
				(< k alength)
			)
			(<= (min main_end) (a k))
		)
	)
)

(conjecture
	(=>
		(and
			(<= 0 alength)
			(forall ((k Int))
				(=>
					(and
						(<= 0 k)
						(< k alength)
					)
					(<= 0 (a k))
				)
			)
		)
		(forall ((k Int))
			(=>
				(and
					(<= 0 k)
					(< k alength)
				)
				(<= (min main_end) (a k))
			)
		)
	)
)
