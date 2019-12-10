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
			(exists ((k Int))
				(and
					(<= 0 k)
					(< k alength)
					(<= (a k) 0)
				)
			)
		)
		(exists ((k Int))
			(and
				(<= 0 k)
				(< k alength)
				(= (a k) (min main_end))
			)
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
					(not (<= (a k) 0))
				)
			)
		)
		(= (min main_end) 0)
	)
)
