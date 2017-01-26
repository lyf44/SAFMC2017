;; Auto-generated. Do not edit!


(when (boundp 'lyf_playground::FloatArray)
  (if (not (find-package "LYF_PLAYGROUND"))
    (make-package "LYF_PLAYGROUND"))
  (shadow 'FloatArray (find-package "LYF_PLAYGROUND")))
(unless (find-package "LYF_PLAYGROUND::FLOATARRAY")
  (make-package "LYF_PLAYGROUND::FLOATARRAY"))

(in-package "ROS")
;;//! \htmlinclude FloatArray.msg.html


(defclass lyf_playground::FloatArray
  :super ros::object
  :slots (_data ))

(defmethod lyf_playground::FloatArray
  (:init
   (&key
    ((:data __data) (make-array 0 :initial-element 0.0 :element-type :float))
    )
   (send-super :init)
   (setq _data __data)
   self)
  (:data
   (&optional __data)
   (if __data (setq _data __data)) _data)
  (:serialization-length
   ()
   (+
    ;; float32[] _data
    (* 4    (length _data)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32[] _data
     (write-long (length _data) s)
     (dotimes (i (length _data))
       (sys::poke (elt _data i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32[] _data
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _data (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _data i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;;
   self)
  )

(setf (get lyf_playground::FloatArray :md5sum-) "420cd38b6b071cd49f2970c3e2cee511")
(setf (get lyf_playground::FloatArray :datatype-) "lyf_playground/FloatArray")
(setf (get lyf_playground::FloatArray :definition-)
      "float32[] data

")



(provide :lyf_playground/FloatArray "420cd38b6b071cd49f2970c3e2cee511")


