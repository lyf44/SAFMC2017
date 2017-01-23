
(cl:in-package :asdf)

(defsystem "lyf_playground-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "IntArray" :depends-on ("_package_IntArray"))
    (:file "_package_IntArray" :depends-on ("_package"))
  ))