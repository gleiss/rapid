//Note: make a and alength const

func main()
{
	const Int[] a;
	Int[] b;
	const Int alength;

	Int i = 0;
	Int m = 0;
	while (i < alength)
	{
		if (a[i] <= a[m])
		{
			b[i] = a[i];
			m = i;
		}
		else
		{
			b[i] = a[m];
		}
		i = i + 1;
	}
}

(assert-not
	(forall ((j Int))
		(exists ((k Int))
			(=>
				(and
					(<= 0 alength)
					(<= 0 j)
					(< j alength)
				)
				(= (b main_end j) (a k))
			)
		)
	)
)
