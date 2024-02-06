<?php
	
	function TheAnswer()
	{
		$x = rand(0,9);

		switch($x)
		{
			case 0:
				return 'Yes.';
			case 1:
				return 'No.';
			case 2:
				return 'Maybe.';
			case 3:
				return 'Who knows?';
			case 4:
				return 'I don\'t know.';
			case 5:
				return 'Does it really matter?';
			case 6:
				return 'Absolutely.';
			case 7:
				return 'No way.';
			case 8:
				return 'It is a possibility.';
			case 9:
				return 'Of course.';
		}
	}
	
	if ($argc != 4)
	{
		echo 'Your input could not be processed';
	}
	else
	{
		echo 'Hello ' . $argv[1] . ' ' . $argv[2] . "!\n";
		echo 'Your question: ' . $argv[3] . "\n";
		echo 'The answer: ' . TheAnswer();
	}
?>