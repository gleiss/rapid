
func main()
{
	const Int[] a;
	const Int alength;

	Int[] b;
	Int blength = 0;

	Int i = 0;
	Int m = 0;
	while (i < alength)
	{
		if (a[i] > a[m])
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
	(forall ((j Int))
		(exists ((k Int))
			(=>
				(and
					(<= 0 alength)
					(<= 0 j)
					(<= 0 k)
					(< j (blength main_end))
					(< k alength)
				)
				(= (b main_end j) (a k))
			)
		)
	)
)

(conjecture
	(forall ((j Int)(k Int))
		(=>
			(and
				(<= 0 alength)
				(<= 0 j)
				(<= 0 k)
				(<= j k)
				(< k (blength main_end))
			)
			(<= (b main_end j) (b main_end k))
		)
	)
)
