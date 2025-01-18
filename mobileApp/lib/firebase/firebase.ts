// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getAnalytics } from "firebase/analytics";
import {getDatabase,onValue,ref} from "firebase/database";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyDbMyJGUj64t760G4ADqNR3Cr2C4gJslu0",
  authDomain: "smartdepartment.firebaseapp.com",
  databaseURL: "https://smartdepartment-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "smartdepartment",
  storageBucket: "smartdepartment.firebasestorage.app",
  messagingSenderId: "455742887143",
  appId: "1:455742887143:web:e2176083279d1df365be42",
  measurementId: "G-9BZY9VXYJX"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const analytics = getAnalytics(app);

export const db = getDatabase();
