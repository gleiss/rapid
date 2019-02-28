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

(assert-not
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
