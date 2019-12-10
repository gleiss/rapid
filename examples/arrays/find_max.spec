
func main()
{
	const Int[] a;
	const Int alength;

	Int i = 0;
	Int max = 0;
	while (i < alength)
	{
		if (a[i] > max)
		{
			max = a[i];
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
			(<= (a k) (max main_end))
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
					(<= 0 (a k))
				)
			)
		)
		(exists ((k Int))
			(and
				(<= 0 k)
				(< k alength)
				(= (a k) (max main_end))
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
					(not (<= 0 (a k)))
				)
			)
		)
		(= (max main_end) 0)
	)
)
