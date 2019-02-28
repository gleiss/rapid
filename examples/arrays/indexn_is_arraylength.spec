// example which shows that we need the requirement that array-lengths are nonnegative: If we remove the precondition "0 <= alength", then neither the standard-encoding nor the timepoint encoding is able to prove the postcondition "i == alength".

func main()
{

	const Int alength;
	Int i = 0;

	while (i < alength)
	{
		i = i + 1;
	}
}

(assert-not
	(= (i main_end) alength)
)
