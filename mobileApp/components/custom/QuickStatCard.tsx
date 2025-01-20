import { View, Text, StyleSheet } from 'react-native';

type QuickStatCardProps = {
    title: string;
    value: number | string;
    subtitle: string;
    color: string;
}

const  QuickStatCard = ({ title, value, subtitle, color }: QuickStatCardProps) => (
    <View style={[styles.quickStatCard, { borderLeftColor: color }]}>
        <Text style={styles.cardTitle}>{title}</Text>
        <Text style={styles.cardValue}>{value}</Text>
        {subtitle && <Text style={styles.cardSubtitle}>{subtitle}</Text>}
    </View>
);

export default QuickStatCard;

const styles = StyleSheet.create({
    quickStatCard: {
        backgroundColor: 'white',
        borderRadius: 12,
        padding: 16,
        width: '100%',
        shadowColor: '#000',
        shadowOffset: { width: 0, height: 1 },
        shadowOpacity: 0.15,
        shadowRadius: 3,
        elevation: 3,
        borderLeftWidth: 4,
        marginBottom: 0,
    },
    cardTitle: {
        fontSize: 13,
        color: '#666',
        marginBottom: 8,
        textTransform: 'uppercase',
        letterSpacing: 0.5,
    },
    cardValue: {
        fontSize: 28,
        fontWeight: '600',
        marginBottom: 8,
        color: '#222',
    },
    cardSubtitle: {
        fontSize: 13,
        color: '#666',
        lineHeight: 18,
    }
});