// find the maximal element in the array
// https://github.com/sosy-lab/sv-benchmarks/blob/master/c/array-examples/sanfoundry_27_true-unreach-call_ground.c

func main()
{
	const Int[] a;
	const Int alength;

	Int i = 1;
	Int max = a[0];
	while (i < alength)
	{
		if (a[i] > max)
		{
			max = a[i];
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
				(<= 0 alength)
				(<= 0 k)
				(< k alength)
			)
			(<= (a k) (max main_end))
		)
	)
)

(conjecture
	(=>
		(and
			(<= 0 alength)
			(forall ((k Int))
				(=>
					(and
						(<= 0 k)
						(< k alength)
					)
					(<= 0 (a k))
				)
			)
		)
		(forall ((k Int))
			(=>
				(and
					(<= 0 k)
					(< k alength)
				)
				(<= (a k) (max main_end))
			)
		)
	)
)
