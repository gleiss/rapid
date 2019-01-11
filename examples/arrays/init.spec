func main()
{
	Int[] a;
	Int i;
	Int alength;
	Int v;

	i=0;
	while(i < alength)
	{
		a[i] = v;
		i = i+1;
	}
}

(assert-not
	(=>
		(>= (alength l8) 0)
		(forall ((pos Int))
			(=>
				(and
					(<= 0 pos)
					(<= pos (alength l8))
				)
				(= (a main_end pos) (v main_end))
			)
		)
	)
)