// Variation 2: The Hamming weight of two identical arrays + 2 elements which have values swapped in their respective traces is identical

(set-traces 2)

func main()
{
	const Int alength;
	const Int[] a;
	const Int x;
	const Int y;

	Int i = 0;
	Int hammingWeight = 0;

	while (i < alength)
	{
		if (a[i] != 0) 
		{
			hammingWeight = hammingWeight + 1;
		}
		else 
		{
			skip;
		}
		i = i + 1;
	}

	if (x != 0) 
	{
		hammingWeight = hammingWeight + 1;
	}
	else 
	{
		skip;
	}

	if (y != 0) 
	{
		hammingWeight = hammingWeight + 1;
	}
	else
	{
		skip;
	}
}

(axiom
	(<= 0 (alength t1))
)
(axiom
	(<= 0 (alength t2))
)

(conjecture
	(=>
		(and
			(= (alength t1) (alength t2))
			(forall ((posA Int))
				(=>
					(and
						(<= 0 posA)
						(< posA (alength t1))
					)
					(= (a posA t1) (a posA t2))
				)
			)
			(= (x t1) (y t2))
			(= (y t1) (x t2))
		)
		(= (hammingWeight main_end t1) (hammingWeight main_end t2))
	)
)

