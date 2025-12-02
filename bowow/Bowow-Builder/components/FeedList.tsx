import React from 'react';
import { View, ScrollView, StyleSheet } from 'react-native';

export default function FeedList({ color }) {
  return (
    <ScrollView contentContainerStyle={styles.container}>
      {[1, 1, 1].map((opacity, index) => (
        <View
          key={index}
          style={[styles.color, { backgroundColor: color, opacity }]}
        />
      ))}
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  color: {
    width: '100%',
    height: 150,
    borderRadius: 25,
    borderCurve: 'continuous',
    marginBottom: 15,
  },
  container: {
    paddingHorizontal: 20,
    paddingVertical: 20,
    height: '100%',
  },
});
