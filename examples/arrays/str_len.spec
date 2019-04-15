func main()
{
	const Int[] a;
	Int i = 0;
	while(a[i] != 0)
	{
		i = i + 1;
	}
}

(conjecture
	(=>
		(exists ((pos Int))
			(and
				(<= 0 pos)
				(= (a pos) 0)
			)
		)
		(forall ((j Int))
			(=>
				(and
					(<= 0 j)
					(< j (i main_end))
				)
				(not (= (a j) 0 ))
			)
		)
	)
)
