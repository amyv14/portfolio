import React, { useState } from "react";
import {
  View,
  Text,
  TouchableOpacity,
  TextInput,
  Alert,
  StyleSheet,
  ImageBackground,
  StatusBar,
  SafeAreaView,
} from "react-native";
import { Link, useRouter } from "expo-router";
import AsyncStorage from '@react-native-async-storage/async-storage';
import * as Constants from '../constants';

const API_URL = Constants.IP_ADDRESS + 'login';

export default function LoginScreen() {
  const router = useRouter();
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [showPassword, setShowPassword] = useState(false);

  // submit function for handling login
  const submit = async () => {
    // check if both fields are filled
    if (!username || !password) {
      Alert.alert('Error', 'Please fill in all fields');
      return;
    }
    try {
      // send login request to the API
      const response = await fetch(API_URL, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ username, password }),
      });
      const data = await response.json();
      // handle successful login response
      if (response.status === 200 && data.token) {
        await AsyncStorage.setItem('token', data.token);
        router.replace('/(tabs)');
      } else {
        // show error if login fails
        Alert.alert('Login failed', 'Invalid username or password');
      }
    } catch (error) {
      console.error(error);
      Alert.alert('Error', 'An error occurred. Please try again.');
    }
  };

  return (
    <ImageBackground
      source={require('../assets/images/gradblue.jpg')}
      style={styles.background}
      resizeMode="cover"
    >
      <StatusBar hidden />
      <SafeAreaView style={styles.safeArea}>
        <View style={styles.formWrapper}>
          <Text style={styles.title}>Login</Text>

          {/* input field for username */}
          <TextInput
            style={styles.input}
            placeholder="Username"
            placeholderTextColor="#DDD"
            value={username}
            onChangeText={setUsername}
            autoCapitalize="none"
          />

          {/* container for password input and show/hide toggle */}
          <View style={styles.passwordContainer}>
            <TextInput
              style={[styles.input, { flex: 1 }]}
              placeholder="Password"
              placeholderTextColor="#DDD"
              value={password}
              onChangeText={setPassword}
              secureTextEntry={!showPassword}
            />
            <TouchableOpacity
              onPress={() => setShowPassword(!showPassword)}
              style={styles.toggleButton}
            >
              {/* button to toggle password visibility */}
              <Text style={styles.toggleText}>
                {showPassword ? 'Hide' : 'Show'}
              </Text>
            </TouchableOpacity>
          </View>

          {/* login button */}
          <TouchableOpacity style={styles.button} onPress={submit}>
            <Text style={styles.buttonText}>Login</Text>
          </TouchableOpacity>

          {/* link to signup page */}
          <Link href="/signup" style={styles.link}>
            <Text style={styles.linkText}>
              Don't have an account? Register Now
            </Text>
          </Link>
        </View>
      </SafeAreaView>
    </ImageBackground>
  );
}

const styles = StyleSheet.create({
  background: {
    flex: 1,
    width: '100%',
    height: '100%',
    justifyContent: 'center',
    alignItems: 'center',
  },
  safeArea: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: 'transparent',
  },
  formWrapper: {
    width: '90%',
    backgroundColor: 'rgba(0, 34, 68, 0.9)',
    padding: 24,
    borderRadius: 20,
    borderWidth: 2,
    borderColor: '#66A3FF',
    shadowColor: '#000',
    shadowOffset: { width: 0, height: 6 },
    shadowOpacity: 0.1,
    shadowRadius: 8,
    elevation: 10,
  },
  title: {
    fontSize: 32,
    color: '#EEFFFF',
    marginBottom: 24,
    fontWeight: '700',
    textAlign: 'center',
  },
  input: {
    height: 50,
    backgroundColor: 'rgba(255, 255, 255, 0.2)',
    borderRadius: 12,
    paddingHorizontal: 16,
    color: '#EEFFFF',
    marginBottom: 16,
    borderWidth: 1,
    borderColor: '#AAD4FF',
    fontSize: 16,
  },
  passwordContainer: {
    flexDirection: 'row',
    alignItems: 'center',
    marginBottom: 16,
  },
  toggleButton: {
    marginLeft: 12,
    padding: 6,
  },
  toggleText: {
    color: '#AAD4FF',
    fontWeight: '600',
    fontSize: 14,
  },
  button: {
    backgroundColor: '#005BBB',
    paddingVertical: 16,
    borderRadius: 12,
    marginTop: 8,
    alignItems: 'center',
  },
  buttonText: {
    color: '#FFF',
    fontSize: 18,
    fontWeight: '700',
  },
  link: {
    marginTop: 16,
    alignItems: 'center',
  },
  linkText: {
    color: '#EEFFFF',
    fontSize: 14,
  },
});
