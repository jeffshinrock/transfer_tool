<?php

	// Used for saving a search result for the user to review later.
	class Course {
		public $transferCourseId;
		public $transferCourseName;
		public $mmuCourseId;
		public $mmuCourseName;

		public function __construct( $transCourseId, $transCourseName, $mmuCourseId, $mmuCourseName ) {
			$this->transferCourseId = $transCourseId;
			$this->transferCourseName = $transCourseName;
			$this->mmuCourseId = $mmuCourseId;
			$this->mmuCourseName = $mmuCourseName;
		}
	}