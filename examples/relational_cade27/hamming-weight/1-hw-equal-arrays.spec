// Variation 1: The Hamming weight of two identical arrays is the same.

(two-traces)
func main()
{
	const Int alength;
	const Int[] a;	
	const Int  x;
	const Int  y;

	Int i = 0;
	Int hammingWeight = 0;

	while (i < alength)
	{
		i = i + 1;
		if (a[i] != 0) {
			hammingWeight = hammingWeight + 1;
		}
		else {
			skip;
		}
	}
}

(conjecture
	(=>
		(and
			(forall ((j Int))
					(= (a j t1) (a j t2))							
			)	
			(= (alength t1) (alength t2))
		)
		(= (hammingWeight main_end t1) (hammingWeight main_end t2))
	)
)

