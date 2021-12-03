<!-- PROJECT LOGO -->
<br />
<p align="center">

  </a>

  <h1 style="font-size:300%;" align="center" >!!BOUNDED BUFFER QUEUE!!</h1>

  <p align="center">
     This is a project that I completed by myself during the
     Fall 2021 semester at California State University, Fresno
    <br />
    <a href="https://www.youtube.com/watch?v=p78f4fwmKoY"><strong>View Demo</a>
  </p>
</p>

<!-- ABOUT THE PROJECT -->
## About The Project
This program displays the following:
  •	Thread creation with thread ID number
  •	Item ID # produced by thread number #
	•	Item ID # consumed by thread number #
	•	Waiting to produce by thread number #
	•	Waiting to consume by thread number #

	-- TP represents the initial sleep time range limit for producing threads
	-- TC represents the initial sleep time range limit for consuming threads.

	This program has the following constraints: <br>
	Producers control their speed of producing by changing the sleeping time <br>
	range between two consecutive produce operations.
	Consumers sleeping time range remains constant throughout program execution
	but consumers stop consuming when the queue is empty.
	The sleep time between calls to producers and consumers is a random value
	within a range (0, T).
	Producers dynamically change speed of "producing" by gradually slowing down
	producing when the queue is over 75% occupancy and stops producing when
	queue is 100% full.
	Producers also gradually accelerate the producing when the queue is below
	25% occupancy and reaches twice the initial sleeping time range when
	buffer is empty.
</p>
## Built With
* C++
* Cygwin
* Visual Studio 2019

<!-- HOW TO USE BOUNDED BUFFER QUEUE -->
## How to use
Using a c++11 compiler.
Running environment is Cygwin64
Demo link: https://www.youtube.com/watch?v=p78f4fwmKoY
Run the program in a Cygwin terminal with the following command:
* g++ -std=c++11 myBBQ.cpp -o BBQ ; ./BBQ.exe TP TC

  The program will run forever until the user stops the program	by pressing ctrl-c.
  The size of the queue MAX is a global variable that can be changed at the time of testing.
  You can change the program to debug mode by changing the DEBUG global variable to true.
  You can change the program to execute in milliseconds rather than seconds by changing MILLI to true

<!-- LICENSE -->
## License
MIT License

<!-- CONTACT -->
## Contact

Eric Smrkovsky - (EricSmrkovsky@mail.fresnostate.edu) - email

<!-- ACKNOWLEDGEMENTS -->
## Acknowledgements

* California State University Fresno, Computer Science Department [http://www.fresnostate.edu/csm/csci/](https://sites.google.com/site/hubertcecotti/home)

<!-- * []() -->

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/EricSmrk/repo.svg?style=for-the-badge
[contributors-url]: https://github.com/NavSanya/AsthmaTravels/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/github_username/repo.svg?style=for-the-badge
[forks-url]: https://github.com/github_username/repo/network/members
[stars-shield]: https://img.shields.io/github/stars/github_username/repo.svg?style=for-the-badge
[stars-url]: https://github.com/github_username/repo/stargazers
[issues-shield]: https://img.shields.io/github/issues/github_username/repo.svg?style=for-the-badge
[issues-url]: https://github.com/github_username/repo/issues
[license-shield]: https://img.shields.io/github/license/github_username/repo.svg?style=for-the-badge
[license-url]: https://github.com/github_username/repo/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/github_username
