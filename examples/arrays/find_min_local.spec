
func main()
{
	const Int[] a;
	const Int alength;

	Int[] b;
	
	b[0] = a[0];
	Int blength = 1;

	Int i = 0;
	Int m = 0;
	while (i < alength)
	{
		if (a[i] < a[m])
		{
			b[i] = a[i];
			blength = blength + 1;
			m = i;
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
				(<= 1 alength)
				(<= 0 k)
				(< k alength)
				(forall ((l Int))
					(=>
						(and
							(<= 0 l)
							(< l k)
						)
						(< (a k) (a l))
					)
				)
			)
			(exists ((j Int))
				(and
					(<= 0 j)
					(< j (blength main_end))
					(= (a k) (b main_end j))
				)
			)
		)
	)
)

(conjecture
	(forall ((j Int))
		(exists ((k Int))
			(=>
				(and
					(<= 1 alength)
					(<= 0 j)
					(< j (blength main_end))
				)
				(and
					(<= 0 k)
					(< k alength)
					(= (b main_end j) (a k))
				)
			)
		)
	)
)

(conjecture
	(forall ((j Int)(k Int))
		(=>
			(and
				(<= 1 alength)
				(<= 0 j)
				(<= 0 k)
				(<= j k)
				(< k (blength main_end))
			)
			(<= (b main_end k) (b main_end j))
		)
	)
)
