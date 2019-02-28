func main()
{
	Int[] a;
	const Int[] b;
	const Int blength;

	 Inti = 0;
	Int alength = 0;
	while(i < blength)
	{
		if (b[i] != 0)
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

(assert-not
	(forall ((k Int))
		(exists ((l Int))
			(=>
				(and
					(<= 0 k)
					(<= 0 blength)
					(< k (alength main_end))
				)
				(= (a main_end k) (b l))
			)
		)
	)
)
