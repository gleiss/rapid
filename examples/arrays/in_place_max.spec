func main()
{
	Int[] a;
	const Int alength;

	Int i = 0;

	while (i < alength)
	{
		if(a[i] < 0)
		{
			a[i] = 0;
		}
		else
		{
			skip;
		}
		i = i + 1;
	}
}

(conjecture
	(forall ((pos Int))
		(=>
			(and
				(<= 0 alength)
				(<= 0 pos)
				(< pos alength)
			)
			(<= 0 (a main_end pos))
		)
	)
)
